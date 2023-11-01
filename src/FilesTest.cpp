#include <globalObjects.hpp>

/* basic file tests */



#ifdef WITH_SD_CARD


String printDirectory(SDFile dir, int numTabs) {
    String out="";
    while (true) {
        SDFile entry =  dir.openNextFile();
        if (! entry) {
            // no more files
            break;
        }

        for (uint8_t i = 0; i < numTabs; i++) {
            out+=('\t');
        }
    
        out+=entry.name();
        if (entry.isDirectory()) {
            out+="/\n";
            out += printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            out+="\t\t";
            out+=String(entry.size(),DEC);
            out+="\n";
        }
        entry.close();
    }
    return out;
}

String fileWriteReadTest(){
    SDFile myFile;
    String out="";

    const char * fileName = F_CONST("test.txt");

    // Check to see if the file exists:
    if (SD.exists(fileName)) {
        out += fileName;
        out += " exists .. will remove it \n";
        SD.remove(fileName);
    } else {
        out += fileName;
        out += " does not exists \n";
    }

    out += "create file for write access \n";
    myFile = SD.open(fileName, FILE_WRITE);

    // create data
    String data="";
    int i,ii;
    for (i=0;i<10;i++){
        for (ii=0; ii<12-i  ;ii++)      data+=' ';
        for (ii=0; ii<1+i*2 ;ii++)      data+='*';
        for (ii=0; ii<12-i  ;ii++)      data+=' ';
        data+='\n';
    }
    for (i=0;i<3;i++){
        for (ii=0; ii<12-1  ;ii++)      data+=' ';
        for (ii=0; ii<1+1*2 ;ii++)      data+='*';
        for (ii=0; ii<12-1  ;ii++)      data+=' ';
        data+='\n';
    }
    data+="merry christams!!\n\n";
    
    // write data to file
    out += "write data to file \n";
    myFile.printf(data.c_str());
    myFile.close();

    // read back
    String res;
    if (SD.exists(fileName)) {
        out += "open file for read access \n";
    } else {
        out += fileName;
        out += " does not exists \n";
        return out;
    }
    myFile = SD.open(fileName, FILE_READ);
    res = myFile.readString();
    myFile.close();

    out += "string length data: ";
    out += String(data.length());
    out += '\n';
    out += "string length file: ";
    out += String(res.length());
    out += '\n';

    if (res == data){
        out += "compare: OK \n";
    } else {
        out += "compare: NOK\n";
        out += "SOLL:\n";
        out += data;
        out += "IST:\n";
        out += res;
    }
    return out;
}

#endif

