/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rsContext.h"
#include "rsProgram.h"

using namespace android;
using namespace android::renderscript;


Program::Program(Context *rsc, Element *in, Element *out) : ObjectBase(rsc)
{
    mAllocFile = __FILE__;
    mAllocLine = __LINE__;

    mElementIn.set(in);
    mElementOut.set(out);
}

Program::~Program()
{
    bindAllocation(NULL);
}


void Program::bindAllocation(Allocation *alloc)
{
    if (mConstants.get() == alloc) {
        return;
    }
    if (mConstants.get()) {
        mConstants.get()->removeProgramToDirty(this);
    }
    mConstants.set(alloc);
    if (alloc) {
        alloc->addProgramToDirty(this);
    }
    mDirty = true;
}

