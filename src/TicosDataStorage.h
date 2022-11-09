#ifndef __TICOS_DATASTORAGE_H
#define __TICOS_DATASTORAGE_H

#include <TicosDevice.h>

class TicosDataStorage: public TicosDevice {
    public:
        TicosDataStorage(){}
        bool open(void) override;
        bool close(void) override;
       // virtual uint32_t save(uint8_t nex=TICOS_HAL_DEFAULT_NEX);
        virtual size_t readfile(const char* path,uint8_t *data,size_t offset,size_t len);
        virtual size_t writefile(size_t filesize,const char* path,uint8_t *data,size_t len);
        virtual size_t appendfile(const char* path,uint8_t *data,size_t len);
        virtual size_t sizefile(const char *path);
        virtual bool renamefile(const char* pathFrom, const char* pathTo);
        virtual bool removefile(const char *path);
        virtual bool createfile(const char *path);
        virtual bool deletefile(const char *path);

};

#endif // __TICOS_WEIGHING_SENSOR_H
