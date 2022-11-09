#include "FS.h"
#include "SPIFFS.h"
#include <TicosDataStorage.h>
#include <Log.h> 

bool TicosDataStorage::open(void)
{
    bool ret;
    const char *mount_point = "/spiffs";
    if (SPIFFS.begin(true, mount_point) ) {
        ret = true;
        logInfo("flash mount ok");
    } else {
        ret = false ;
        logInfo("flash mount fail");
    }
    return ret;
}

bool TicosDataStorage::close(void)
{
    SPIFFS.end();
    return 1;
}

size_t TicosDataStorage::readfile(const char* path,uint8_t *data,size_t offset,size_t len)
{
    size_t file_size;
    size_t rd_len;

    if(!len){
        return 0;
    }
    File file = SPIFFS.open(path,FILE_READ);
    logInfo("read file,offset:%d,len:%d",offset,len);

    if (!file) {
        //raiseError(ctx); // Can't open file
        logDebug("open file failed !");
        return 0;
    }
    file_size = file.size();
    if(offset > file_size){
        logDebug("error:offset out of file size !");
        return 0;
    }
    file.seek(offset,SeekCur);
    rd_len = file.read(data,len);
    file.close();

    return rd_len ;
}

size_t TicosDataStorage::writefile(size_t filesize,const char* path,uint8_t *data,size_t len) 
{
    size_t cursor;
    size_t wr_len;

    if(!len){
        return  0;
    }
    File file = SPIFFS.open(path,FILE_WRITE);
    if (!file) {
       // raiseError(ctx); // Can't open file
        return 0;
    }
    cursor = file.size();
    logInfo("file limit size:%d,len:%d,cursor:0x%08x\n",filesize,len,cursor);
    //如果filesize大于0，则限制文件大小，防止flash被写满
    if((filesize != 0) && (cursor >= filesize))
    {
        file.seek(0);
        logInfo("cursor : 0\n");
    }
    else
    {
        file.seek(cursor);
        logInfo("cursor : %d\n",cursor);
    }

    file.write(data,len);
    file.flush();
    file.close();
    return len ;
}

size_t TicosDataStorage::appendfile(const char* path,uint8_t *data,size_t len) 
{
    size_t file_size;
    size_t wr_len;

    if(!len){
        return  0;
    }
    File file = SPIFFS.open(path,FILE_APPEND);
    logInfo("write file,len:%d",len);

    if (!file) {
       // raiseError(ctx); // Can't open file
        return 0;
    }
    file.write(data,len);
    file.close();
    return len ;
}

size_t TicosDataStorage::sizefile(const char *path)
{
    size_t file_size;

    File file = SPIFFS.open(path,FILE_READ); 
    if (!file) {
        return 0;
    }
    file_size = file.size();
    file.close();

    return file_size ;
}

bool TicosDataStorage::renamefile(const char* pathFrom, const char* pathTo)
{
   if (SPIFFS.rename(pathFrom, pathTo)) {
        logInfo("File renamed");
    } else {
        logInfo("Rename failed");
    }
}
bool TicosDataStorage::removefile(const char *path)
{
    if(SPIFFS.rmdir(path)){
    logInfo("Dir removed");
    } else {
    logInfo("rmdir failed");
    }
}

bool TicosDataStorage::createfile(const char *path)
{

}
bool TicosDataStorage::deletefile(const char *path)
{
    if(SPIFFS.remove(path)){
        logInfo("File deleted");
    } else {
        logInfo("Delete failed");
    }
}

