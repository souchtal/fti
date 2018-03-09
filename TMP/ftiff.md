## FTI File Format (FTI-FF)
### Structure

The file format basic structure, consists of a meta block and a data block:
```
   +--------------+ +------------------------+
   |              | |                        |
   | FDB          | | VDB                    |
   |              | |                        |
   +--------------+ +------------------------+
```
The `FDB` block holds meta data related to the file whereas the `VDB` block holds meta data and actual data of the variables protected by FTI.

The `FDB` block has the following structure:
```c++
   FDB {
       checksum        // Hash of the VDB block stored in hex representation (33 bytes) 
       hash            // Hash of
       ckptSize
       fs
       maxFs
       ptFs
       timestamp
   }
```

