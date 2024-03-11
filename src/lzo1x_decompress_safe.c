#include "lzo1x_decompress_safe.h"

int lzo1x_decompress_safe(const unsigned char *in, size_t in_len,
			  unsigned char *out, size_t *out_len)
{
	
	unsigned char *op; // Pointer from out
	const unsigned char *ip; // Pointer from in
	size_t t, next;
	size_t state = 0;
	const unsigned char *m_pos;
	const unsigned char * const ip_end = in + in_len;
	unsigned char * const op_end = out + *out_len;

	unsigned char bitstream_version;

	op = out; // Set op to out buffer
	ip = in;  // Set ip to in buffer

	if (unlikely(in_len < 3)) // size of comp info must be more than 3 bytes
		goto input_overrun;

	if (likely(in_len >= 5) && likely(*ip == 17)) { // set bitstream if start of pointer is 17.
		bitstream_version = ip[1];
		ip += 2; // jump 2 next
	} else {
		bitstream_version = 0;
	}

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4) {
			next = t;
			goto match_next;
		}
		goto copy_literal_run;
	}

	for (;;) {
		t = *ip++; // read byte and go forward. In most cases ip in that position is a last byte in instruction.
		if (t < 16) {
			if (likely(state == 0)) {
				if (unlikely(t == 0)) {
					size_t offset;
					const unsigned char *ip_last = ip;

					while (unlikely(*ip == 0)) {
						ip++;
						NEED_IP(1);
					}
					offset = ip - ip_last;
					if (unlikely(offset > MAX_255_COUNT))
						return LZO_E_ERROR;

					offset = (offset << 8) - offset;
					t += offset + 15 + *ip++; // t is a nearest compressied information
				}
				t += 3;
copy_literal_run:
				{
					NEED_OP(t);
					NEED_IP(t + 3);
					do {
						*op++ = *ip++;
					} while (--t > 0); // program start with searching a place where program can find a comprassied information. Also they copy in buffer to out untill program find comprassied info.
				}
				state = 4;
				continue;
			} else if (state != 4) {
				next = t & 3; 
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				TEST_LB(m_pos);
				NEED_OP(2);
				op[0] = m_pos[0]; // copy first byte
				op[1] = m_pos[1]; // copy second byte
				op += 2; // pointer -> 2
				goto match_next;
			} else {
				next = t & 3;
				m_pos = op - (1 + M2_MAX_OFFSET);
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				t = 3;
			}
		} else if (t >= 64) {
			next = t & 3; // this is S in 0 1 L D D D S S
			// m_pos is a distance = (H << 3) + D + 1
			m_pos = op - 1; // This is plus one in formula
			m_pos -= (t >> 2) & 7; // This is D in formule 
			m_pos -= *ip++ << 3; // This is H << 3 in formula 
			t = (t >> 5) - 1 + (3 - 1); // Len, which Len = 3 + L
		} else if (t >= 32) {
			t = (t & 31) + (3 - 1); // t & 31 is just a mask of 0 0 1 L L L L L, where added +2
			// this is just a formula 2 + L, from lzo.txt.
			if (unlikely(t == 2)) {
				size_t offset;
				const unsigned char *ip_last = ip;

				while (unlikely(*ip == 0)) {
					ip++;
					NEED_IP(1);
				}
				offset = ip - ip_last;
				if (unlikely(offset > MAX_255_COUNT))
					return LZO_E_ERROR;

				offset = (offset << 8) - offset;
				t += offset + 31 + *ip++;
				NEED_IP(2);
			}
			m_pos = op - 1;
			next = get_unaligned_le16(ip); // read 2 bytes in little endian
			ip += 2; // ip pointer go to end of coded information(to last byte)
			m_pos -= next >> 2; // go to reference (<-) 
			next &= 3;
		} else {
			NEED_IP(2);
			next = get_unaligned_le16(ip);
			if (((next & 0xfffc) == 0xfffc) &&
			    ((t & 0xf8) == 0x18) &&
			    likely(bitstream_version)) {
				NEED_IP(3);
				t &= 7;
				t |= ip[2] << 3;
				t += MIN_ZERO_RUN_LENGTH;
				NEED_OP(t);
				memset(op, 0, t);
				op += t;
				next &= 3;
				ip += 3;
				goto match_next;
			} else {
				m_pos = op;
				m_pos -= (t & 8) << 11;
				t = (t & 7) + (3 - 1);
				if (unlikely(t == 2)) {
					size_t offset;
					const unsigned char *ip_last = ip;

					while (unlikely(*ip == 0)) {
						ip++;
						NEED_IP(1);
					}
					offset = ip - ip_last;
					if (unlikely(offset > MAX_255_COUNT))
						return LZO_E_ERROR;

					offset = (offset << 8) - offset;
					t += offset + 7 + *ip++;
					NEED_IP(2);
					next = get_unaligned_le16(ip);
				}
				ip += 2;
				m_pos -= next >> 2;
				next &= 3;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			}
		}
		TEST_LB(m_pos);
		{
			unsigned char *oe = op + t; // here programm copy from reference information: do from compared text to source
			NEED_OP(t);
			op[0] = m_pos[0]; // save first byte
			op[1] = m_pos[1]; // save second byte
			op += 2; // always it's more than 2 bytes.
			m_pos += 2; // pointer from source to -> +2
			do {
				*op++ = *m_pos++; // copy all bytes
			} while (op < oe); // untill op reach oe
		}
match_next:
		state = next;
		t = next;
		{
			NEED_IP(t + 3);
			NEED_OP(t);
			while (t > 0) {
				*op++ = *ip++;
				t--;
			}
		}
	}

eof_found:
	*out_len = op - out;
	return (t != 3       ? LZO_E_ERROR :
		ip == ip_end ? LZO_E_OK :
		ip <  ip_end ? LZO_E_INPUT_NOT_CONSUMED : LZO_E_INPUT_OVERRUN);

input_overrun:
	*out_len = op - out;
	return LZO_E_INPUT_OVERRUN;

output_overrun:
	*out_len = op - out;
	return LZO_E_OUTPUT_OVERRUN;

lookbehind_overrun:
	*out_len = op - out;
	//printf("*out_len: %lu\n op and out: %d and %d \n", *out_len, op, out);
	return LZO_E_LOOKBEHIND_OVERRUN;
}
