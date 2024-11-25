
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once
#include "ComFrame.hpp"
#include <Ctrl.hpp>


class FileTransferState {
public:
    String filename;
    uint32_t fileSize = 0;
    uint32_t totalChunks = 0;
    uint32_t currentChunk = 0;
    bool isActive = false;

    void reset() {
        filename = "";
        fileSize = 0;
        totalChunks = 0;
        currentChunk = 0;
        isActive = false;
    }
};


#define MAX_FILE_CHUNK_SIZE 128 // Maximum file chunk size for splitting.

class ComDispatch
{
public:
	ComDispatch();
	~ComDispatch() = default;

    bool dispatchFrame(ComFrame * pFrame);

private:
    bool _dump(ComFrame * pFrame);
    bool _dispatchLedFrame(ComFrame * pFrame);
    bool _dispatchCommonFrame(ComFrame * pFrame);
    bool _readFile(ComFrame * pFrame);
    bool _writeFile(ComFrame * pFrame);

    Ctrl * _refToObj(char module,uint8_t index);

    FileTransferState _fileTransferState;
};
