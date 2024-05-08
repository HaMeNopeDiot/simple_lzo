# Simple LZO

This is shell for lzo linux realization. Make some changes for working from console.
Just set the input path file with arg ```-i```, output path with arg ```-o``` and programm will be compress file and put it in output path. If you want to decompress file use arg ```-d``` instead.
For help use ```-h``` flag. Below you can see list of arguments:
 ```
 Help note, how to use this prog\n"
       -h            help\n"
       -t            testing by compress and decompress\n"
       -d            decompress mode. If uncheck, programm will enable compress mode\n"
       -i            input file\n"
       -o            output file\n"
       -v            verbose\n"
       -b            bitstream\n"
       -s            decompress mode from lzo simple (may unstable)\n"
```
In this code have two realizations for decompressor. One of them this from Kernel code, another myself realization in much more simple code. Both of them I tested on few different examples, it's kinda works, but I don't can say, that it can't broke.
This algorithm working good if compress text data, yaml or xml files. In some cases algorithm can compress data into bigger value that was be.

Also I used file "LZO stream format as understood by Linux's LZO decompressor" for understanding of work decompressor.

Tested on Linux & Windows (with MinGW)
## How to use

To run this variant of lzo need to run in console:
```
make
```
After build, you can use it. For get help note, try this:
```
./lzo1x -h
```
For compress something you can just using ```-i``` argument put path to compressiable file. This program for compressed file have output default path and name. But I recommend to choose output path for file, using ```-o```.
If you want to decompress file, you need to use ```-d``` argument.

## Possible problems

Besides of getting bigger compressed data than source size data, data program can crash. If compressed data be more than ```2 * source size data```, compressor just get error, because no more memory is allocated to the output source than the data of the source size times twice.
