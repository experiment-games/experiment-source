// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: petar@google.com (Petar Petrov)

#ifndef GOOGLE_PROTOBUF_PYTHON_CPP_DESCRIPTOR_H__
#define GOOGLE_PROTOBUF_PYTHON_CPP_DESCRIPTOR_H__

#include <Python.h>
#include <structmember.h>

#include <google/protobuf/descriptor.h>

#if PY_VERSION_HEX < 0x02050000 && !defined( PY_SSIZE_T_MIN )
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

namespace google
{
namespace protobuf
{
namespace python
{

typedef struct CFieldDescriptor
{
    PyObject_HEAD

        // The proto2 descriptor that this object represents.
        const google::protobuf::FieldDescriptor* descriptor;

    // Reference to the original field object in the Python DESCRIPTOR.
    PyObject* descriptor_field;
} CFieldDescriptor;

typedef struct
{
    PyObject_HEAD

        const google::protobuf::DescriptorPool* pool;
} CDescriptorPool;

extern PyTypeObject CFieldDescriptor_Type;

extern PyTypeObject CDescriptorPool_Type;

namespace cdescriptor_pool
{

// Looks up a field by name. Returns a CDescriptor corresponding to
// the field on success, or NULL on failure.
//
// Returns a new reference.
PyObject* FindFieldByName( CDescriptorPool* self, PyObject* name );

// Looks up an extension by name. Returns a CDescriptor corresponding
// to the field on success, or NULL on failure.
//
// Returns a new reference.
PyObject* FindExtensionByName( CDescriptorPool* self, PyObject* arg );

}  // namespace cdescriptor_pool

PyObject* Python_NewCDescriptorPool( PyObject* ignored, PyObject* args );
PyObject* Python_BuildFile( PyObject* ignored, PyObject* args );
bool InitDescriptor();
google::protobuf::DescriptorPool* GetDescriptorPool();

}  // namespace python
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_PYTHON_CPP_DESCRIPTOR_H__
