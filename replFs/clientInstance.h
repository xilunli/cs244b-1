#ifndef CLIENTINSTANCE_H
#define CLIENTINSTANCE_H

#include "string.h"
#include "sys/time.h"
#include <set>
#include <map>

#include "networkInstance.h"

/* Timeouts in ms */
#define OPENFILE_TIMEOUT 6000
#define OPENFILE_RETRY_TIMEOUT 200

#define COMMITPREPARE_TIMEOUT 6000
#define COMMITPREPARE_RETRY_TIMEOUT 200

#define COMMIT_TIMEOUT 6000
#define COMMIT_RETRY_TIMEOUT 200

#define CLOSE_TIMEOUT 6000
#define CLOSE_RETRY_TIMEOUT 200

class ClientInstance {
private:
    NetworkInstance *N;
    unsigned int numServers;
    std::map<uint32_t, PacketWriteBlock> blocks;
    typedef std::map<uint32_t, PacketWriteBlock>::iterator mapit;
    typedef std::set<uint32_t>::iterator blockIDit;
    bool fileOpened;
    int curFd;
    int curBlockID;
    timeval timeOpenFile;
    timeval timeOpenFileRetry;
    timeval timeCommitPrepare;
    timeval timeCommitPrepareRetry;
    timeval timeCommit;
    timeval timeCommitRetry;
    timeval timeClose;
    timeval timeCloseRetry;

public:
    ClientInstance(unsigned short _port, int _droprate, int _numServers) {
        numServers = _numServers;
        fileOpened = false;
        curFd = 1;
        curBlockID = 1;
        N = new NetworkInstance(_port, FS_GROUP, _droprate, false);
    };

    ~ClientInstance() {
        delete N;
    }
    int openFile(char *strFileName);
    int writeBlock(int fd, char *strData, int byteOffset, int blockSize);
    int commit(int fd);
    int abort(int fd);
    int closeFile(int fd);

private:
    int commitFinal();
    void setCurrentTime(timeval *t);
    bool isTimeout(timeval startup, long timeout);
    long timediff(timeval t1, timeval t2);
};

#endif
