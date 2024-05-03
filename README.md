This is shell for lzo linux realization. Some changes for working in console.
Just set input path file, output and programm will be compress or decompress file.
For help use -h flag. Below you can see list of arguments:
 
        "Help note, how to use this prog\n"
        "   -h            help\n"
        "   -t            testing by compress and decompress\n"
        "   -d            decompress mode. If uncheck, programm will enable compress mode\n"
        "   -i            input file\n"
        "   -o            output file\n"
        "   -v            verbose\n"
        "   -b            bitstream\n"
        "   -s            decompress mode from lzo simple (may unstable)\n"

In this code you can see two realizations for decompressor. One of them this from Kernel code, another myself realization in simple code. Both of them I tested on few examples, it's kinda works, but I don't can say, that it can broke.
This algorithm working good if compress text data, yaml or xml files. In some cases algorithm can compress data into bigger value that was be.
