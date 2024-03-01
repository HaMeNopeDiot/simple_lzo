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
		t = *ip++; // Usually it is a first byte in instruction
		if (t < 16) {
			if (likely(state == 0)) { // no literal copy
				if (unlikely(t == 0)) { // if t is null
					size_t offset;
					const unsigned char *ip_last = ip;

					while (unlikely(*ip == 0)) {
						ip++; // skip null bytes
						NEED_IP(1);
					}
					offset = ip - ip_last; // Usually zero, because in text files \ 
					// Distance between instructions is lower than 255.
					if (unlikely(offset > MAX_255_COUNT))
						return LZO_E_ERROR;

					offset = (offset << 8) - offset; /* This is offset for getting bigger \ 
					values of distance between instructions. Even if offset is 1, distance \ 
					between instructions will be increased to 255 */
					t += offset + 15 + *ip++; // t is a nearest next instruction
				}
				t += 3; // result distance between instructions
copy_literal_run:
				{
					NEED_OP(t);
					NEED_IP(t + 3);
					do {
						*op++ = *ip++; // skip usual bytes
					} while (--t > 0); // program start with searching a place where program can find a comprassied information. Also they copy in buffer to out untill program find comprassied info.
				}
				state = 4;	
				continue; // GO BACK
			} else if (state != 4) { // if last instruction didn't copy 4 literals
				next = t & 3; // this is a S in 0 0 0 0 D D S S
				// m_pos is a distance equal (H << 2) + D + 1
				m_pos = op - 1;  // is a +1
				m_pos -= t >> 2; // is a D
				m_pos -= *ip++ << 2; // is a H << 2
				TEST_LB(m_pos);
				NEED_OP(2);
				op[0] = m_pos[0]; // copy first byte
				op[1] = m_pos[1]; // copy second byte
				op += 2; // pointer -> 2
				goto match_next;
			} else {
				next = t & 3; // this is a S in [0 0 0 0 D D S S]
				m_pos = op - (1 + M2_MAX_OFFSET); // calc distance between 2..3kB
				m_pos -= t >> 2; // is a D
				m_pos -= *ip++ << 2; // is a (H << 2)
				t = 3; // 3 bytes to copy.
			}
		} else if (t >= 64) {
			next = t & 3; // this is S in [0 1 L D D D S S]
			// m_pos is a distance = (H << 3) + D + 1
			m_pos = op - 1; // This is plus one in formula
			m_pos -= (t >> 2) & 7; // This is D in formule 
			m_pos -= *ip++ << 3; // This is H << 3 in formula 
			t = (t >> 5) - 1 + (3 - 1); // Len, which Len = 3 + L
		} else if (t >= 32) {
			t = (t & 31) + (3 - 1); // t & 31 is just a mask of 0 0 1 L L L L L, where added +2
			// this is just a formula 2 + L, from lzo.txt.
			if (unlikely(t == 2)) { /* if  L equal zero, then L is more than 31 \
				AND, we need to count offset, that will be count zero bytes and use it to \
				count real L. */
				size_t offset;
				const unsigned char *ip_last = ip;

				while (unlikely(*ip == 0)) { // count and skip null bytes
					ip++; // skip zero byte
					NEED_IP(1);
				}
				offset = ip - ip_last; // calc offset (count zero bytes) 
				if (unlikely(offset > MAX_255_COUNT))
					return LZO_E_ERROR;

				offset = (offset << 8) - offset; // count a offset. Remember, that if \ 
				// offset is 1, then offset be equal 255.
				t += offset + 31 + *ip++;  // if t is 2, and we have 31, we can additional count \ 
				// from *ip up to 255 value.
				NEED_IP(2);
			}
			m_pos = op - 1; // this is a +1 in formula "distance = D + 1"
			next = get_unaligned_le16(ip); // read 2 bytes in little endian
			ip += 2; // ip pointer go to end of coded information(to last byte)
			m_pos -= next >> 2; // this is D in formula "distance = D + 1"
			next &= 3; // set state
		} else { // if t < 32 or simple language in range 16..31
			NEED_IP(2);
			next = get_unaligned_le16(ip); /* read LE16 2 bytes in format \ 
			D D D D D D D D : D D D D D D S S */
			if (((next & 0xfffc) == 0xfffc) && //if all D is maximum,
			    ((t & 0xf8) == 0x18) && // and H is 1
			    likely(bitstream_version)) { // and bitstream is 1
				NEED_IP(3);
				// for calc length uses formula: "((X << 3) | (0 0 0 0 0 L L L)) + 4"
				t &= 7; // read L
				t |= ip[2] << 3; // ip is a X
				t += MIN_ZERO_RUN_LENGTH; // it is a +4 in usual case
				NEED_OP(t);
				memset(op, 0, t);
				op += t; // 
				next &= 3; //set state to mask 3 (state can be between 0 and 2)
				ip += 3; /* jump forward to 3. (ip[0] and ip [1] used for le16) \
				ip[2] used for X */
				goto match_next;
			} else {
				// Format for 16..31 is a [0 0 0 1 H L L L]
				// formula for distance = 16384 + (H << 14) + D
				m_pos = op; // this is a D
				m_pos -= (t & 8) << 11; // this is a (H << 14)
				t = (t & 7) + (3 - 1); // this is a formule: length = L + 2
				if (unlikely(t == 2)) { // if L is 0, then we need to count null bytes
					size_t offset;
					const unsigned char *ip_last = ip; // set ip_last to start count nulls

					while (unlikely(*ip == 0)) { // skip null bytes, untill see not null byte
						ip++; // skip byte
						NEED_IP(1);
					}
					offset = ip - ip_last; // calc count null bytes
					if (unlikely(offset > MAX_255_COUNT))
						return LZO_E_ERROR;

					offset = (offset << 8) - offset; /* Calc offset, if offset 1, then new
					offset must be 255 */
					t += offset + 7 + *ip++; // count real length
					NEED_IP(2);
					next = get_unaligned_le16(ip); // and now read two le16 bytes
				}
				ip += 2; // after read we need to move pointer(other words skip le16).
				m_pos -= next >> 2; // read D
				next &= 3; // read S
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000; // 0x4000 is a +16384
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
		state = next; // saving state
		t = next; 
		{
			NEED_IP(t + 3);
			NEED_OP(t);
			while (t > 0) {
				*op++ = *ip++; // skip all bytes to next instruction?
				t--;
			}
		}
	}

eof_found: // searching pointer reach the eof file in some reason
	*out_len = op - out;
	return (t != 3       ? LZO_E_ERROR :
		ip == ip_end ? LZO_E_OK :
		ip <  ip_end ? LZO_E_INPUT_NOT_CONSUMED : LZO_E_INPUT_OVERRUN);

input_overrun: // too small input
	*out_len = op - out;
	return LZO_E_INPUT_OVERRUN;

output_overrun: // dont used
	*out_len = op - out;
	return LZO_E_OUTPUT_OVERRUN;

lookbehind_overrun: // dont used
	*out_len = op - out;
	return LZO_E_LOOKBEHIND_OVERRUN;
}
