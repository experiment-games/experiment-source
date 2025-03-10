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

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <google/protobuf/compiler/java/java_message.h>

#include <algorithm>
#include <google/protobuf/stubs/hash.h>
#include <map>
#include <memory>
#include <vector>

#include <google/protobuf/compiler/java/java_context.h>
#include <google/protobuf/compiler/java/java_doc_comment.h>
#include <google/protobuf/compiler/java/java_enum.h>
#include <google/protobuf/compiler/java/java_extension.h>
#include <google/protobuf/compiler/java/java_generator_factory.h>
#include <google/protobuf/compiler/java/java_helpers.h>
#include <google/protobuf/compiler/java/java_name_resolver.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/stubs/substitute.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace java {

using internal::WireFormat;
using internal::WireFormatLite;

namespace {
bool GenerateHasBits(const Descriptor* descriptor) {
  return SupportFieldPresence(descriptor->file()) ||
    HasRepeatedFields(descriptor);
}
}  // namespace

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor)
  : descriptor_(descriptor) {}

MessageGenerator::~MessageGenerator() {}

// ===================================================================
// TODO(api): Move this class to a separate immutable_message.cc file.
ImmutableMessageGenerator::ImmutableMessageGenerator(
    const Descriptor* descriptor, Context* context)
  : MessageGenerator(descriptor), context_(context),
    name_resolver_(context->GetNameResolver()),
    field_generators_(descriptor, context_) {
}

ImmutableMessageGenerator::~ImmutableMessageGenerator() {}

void ImmutableMessageGenerator::GenerateStaticVariables(io::Printer* printer) {
  if (HasDescriptorMethods(descriptor_)) {
    // Because descriptor.proto (com.google.protobuf.DescriptorProtos) is
    // used in the construction of descriptors, we have a tricky bootstrapping
    // problem.  To help control static initialization order, we make sure all
    // descriptors and other static data that depends on them are members of
    // the outermost class in the file.  This way, they will be initialized in
    // a deterministic order.

    map<string, string> vars;
    vars["identifier"] = UniqueFileScopeIdentifier(descriptor_);
    vars["index"] = SimpleItoa(descriptor_->index());
    vars["classname"] = name_resolver_->GetImmutableClassName(descriptor_);
    if (descriptor_->containing_type() != NULL) {
    vars["parent"] = UniqueFileScopeIdentifier(
        descriptor_->containing_type());
    }
    if (MultipleJavaFiles(descriptor_->file(), /* immutable = */ true)) {
    // We can only make these package-private since the classes that use them
    // are in separate files.
    vars["private"] = "";
    } else {
    vars["private"] = "private ";
    }

    // The descriptor for this type.
    printer->Print(vars,
    "$private$static final com.google.protobuf.Descriptors.Descriptor\n"
    "  internal_$identifier$_descriptor;\n");

    // And the FieldAccessorTable.
    GenerateFieldAccessorTable(printer);
  }

  // Generate static members for all nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    // TODO(kenton):  Reuse MessageGenerator objects?
    ImmutableMessageGenerator(descriptor_->nested_type(i), context_)
    .GenerateStaticVariables(printer);
  }
}

void ImmutableMessageGenerator::GenerateStaticVariableInitializers(
    io::Printer* printer) {
  if (HasDescriptorMethods(descriptor_)) {
    map<string, string> vars;
    vars["identifier"] = UniqueFileScopeIdentifier(descriptor_);
    vars["index"] = SimpleItoa(descriptor_->index());
    vars["classname"] = name_resolver_->GetImmutableClassName(descriptor_);
    if (descriptor_->containing_type() != NULL) {
    vars["parent"] = UniqueFileScopeIdentifier(
        descriptor_->containing_type());
    }

    // The descriptor for this type.
    if (descriptor_->containing_type() == NULL) {
    printer->Print(vars,
        "internal_$identifier$_descriptor =\n"
        "  getDescriptor().getMessageTypes().get($index$);\n");
    } else {
    printer->Print(vars,
        "internal_$identifier$_descriptor =\n"
        "  internal_$parent$_descriptor.getNestedTypes().get($index$);\n");
    }

    // And the FieldAccessorTable.
    GenerateFieldAccessorTableInitializer(printer);
  }

  // Generate static member initializers for all nested types.
  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    // TODO(kenton):  Reuse MessageGenerator objects?
    ImmutableMessageGenerator(descriptor_->nested_type(i), context_)
    .GenerateStaticVariableInitializers(printer);
  }
}

void ImmutableMessageGenerator::
GenerateFieldAccessorTable(io::Printer* printer) {
  map<string, string> vars;
  vars["identifier"] = UniqueFileScopeIdentifier(descriptor_);
  if (MultipleJavaFiles(descriptor_->file(), /* immutable = */ true)) {
    // We can only make these package-private since the classes that use them
    // are in separate files.
    vars["private"] = "";
  } else {
    vars["private"] = "private ";
  }
  printer->Print(vars,
    "$private$static\n"
    "  com.google.protobuf.GeneratedMessage.FieldAccessorTable\n"
    "    internal_$identifier$_fieldAccessorTable;\n");
}

void ImmutableMessageGenerator::
GenerateFieldAccessorTableInitializer(io::Printer* printer) {
  printer->Print(
    "internal_$identifier$_fieldAccessorTable = new\n"
    "  com.google.protobuf.GeneratedMessage.FieldAccessorTable(\n"
    "    internal_$identifier$_descriptor,\n"
    "    new java.lang.String[] { ",
    "identifier",
    UniqueFileScopeIdentifier(descriptor_));
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    const FieldGeneratorInfo* info = context_->GetFieldGeneratorInfo(field);
    printer->Print(
    "\"$field_name$\", ",
    "field_name", info->capitalized_name);
  }
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    const OneofDescriptor* oneof = descriptor_->oneof_decl(i);
    const OneofGeneratorInfo* info = context_->GetOneofGeneratorInfo(oneof);
    printer->Print(
    "\"$oneof_name$\", ",
    "oneof_name", info->capitalized_name);
  }
  printer->Print("});\n");
}

// ===================================================================

void ImmutableMessageGenerator::GenerateInterface(io::Printer* printer) {
  if (descriptor_->extension_range_count() > 0) {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public interface $classname$OrBuilder extends\n"
        "    $extra_interfaces$\n"
        "    com.google.protobuf.GeneratedMessage.\n"
        "        ExtendableMessageOrBuilder<$classname$> {\n",
        "extra_interfaces", ExtraMessageOrBuilderInterfaces(descriptor_),
        "classname", descriptor_->name());
    } else {
    printer->Print(
        "public interface $classname$OrBuilder extends \n"
        "    $extra_interfaces$\n"
        "     com.google.protobuf.GeneratedMessageLite.\n"
        "          ExtendableMessageOrBuilder<$classname$> {\n",
        "extra_interfaces", ExtraMessageOrBuilderInterfaces(descriptor_),
        "classname", descriptor_->name());
    }
  } else {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public interface $classname$OrBuilder extends\n"
        "    $extra_interfaces$\n"
        "    com.google.protobuf.MessageOrBuilder {\n",
        "extra_interfaces", ExtraMessageOrBuilderInterfaces(descriptor_),
        "classname", descriptor_->name());
    } else {
    printer->Print(
        "public interface $classname$OrBuilder extends\n"
        "    $extra_interfaces$\n"
        "    com.google.protobuf.MessageLiteOrBuilder {\n",
        "extra_interfaces", ExtraMessageOrBuilderInterfaces(descriptor_),
        "classname", descriptor_->name());
    }
  }

  printer->Indent();
    for (int i = 0; i < descriptor_->field_count(); i++) {
    printer->Print("\n");
    field_generators_.get(descriptor_->field(i))
                        .GenerateInterfaceMembers(printer);
    }
  printer->Outdent();

  printer->Print("}\n");
}

// ===================================================================

void ImmutableMessageGenerator::Generate(io::Printer* printer) {
  bool is_own_file =
    descriptor_->containing_type() == NULL &&
    MultipleJavaFiles(descriptor_->file(), /* immutable = */ true);

  WriteMessageDocComment(printer, descriptor_);

  // The builder_type stores the super type name of the nested Builder class.
  string builder_type;
  if (descriptor_->extension_range_count() > 0) {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public$static$final class $classname$ extends\n"
        "    com.google.protobuf.GeneratedMessage.ExtendableMessage<\n"
        "      $classname$> implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "static", is_own_file ? " " : " static ",
        "classname", descriptor_->name(),
        "extra_interfaces", ExtraMessageInterfaces(descriptor_));
    builder_type = strings::Substitute(
        "com.google.protobuf.GeneratedMessage.ExtendableBuilder<$0, ?>",
        name_resolver_->GetImmutableClassName(descriptor_));
    } else {
    printer->Print(
        "public$static$final class $classname$ extends\n"
        "    com.google.protobuf.GeneratedMessageLite.ExtendableMessage<\n"
        "      $classname$> implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "static", is_own_file ? " " : " static ",
        "classname", descriptor_->name(),
        "extra_interfaces", ExtraMessageInterfaces(descriptor_));
    builder_type = strings::Substitute(
        "com.google.protobuf.GeneratedMessageLite.ExtendableBuilder<$0, ?>",
        name_resolver_->GetImmutableClassName(descriptor_));
    }
  } else {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public$static$final class $classname$ extends\n"
        "    com.google.protobuf.GeneratedMessage implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "static", is_own_file ? " " : " static ",
        "classname", descriptor_->name(),
        "extra_interfaces", ExtraMessageInterfaces(descriptor_));
    builder_type = "com.google.protobuf.GeneratedMessage.Builder<?>";
    } else {
    printer->Print(
        "public$static$final class $classname$ extends\n"
        "    com.google.protobuf.GeneratedMessageLite implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "static", is_own_file ? " " : " static ",
        "classname", descriptor_->name(),
        "extra_interfaces", ExtraMessageInterfaces(descriptor_));
    builder_type = "com.google.protobuf.GeneratedMessageLite.Builder";
    }
  }
  printer->Indent();
  // Using builder_type, instead of Builder, prevents the Builder class from
  // being loaded into PermGen space when the default instance is created.
  // This optimizes the PermGen space usage for clients that do not modify
  // messages.
  printer->Print(
    "// Use $classname$.newBuilder() to construct.\n"
    "private $classname$($buildertype$ builder) {\n"
    "  super(builder);\n"
    "$set_unknown_fields$\n"
    "}\n",
    "classname", descriptor_->name(),
    "buildertype", builder_type,
    "set_unknown_fields",
    "  this.unknownFields = builder.getUnknownFields();");
  printer->Print(
    // Used when constructing the default instance, which cannot be initialized
    // immediately because it may cyclically refer to other default instances.
    "private $classname$(boolean noInit) {$set_default_unknown_fields$}\n"
    "\n"
    "private static final $classname$ defaultInstance;\n"
    "public static $classname$ getDefaultInstance() {\n"
    "  return defaultInstance;\n"
    "}\n"
    "\n"
    "public $classname$ getDefaultInstanceForType() {\n"
    "  return defaultInstance;\n"
    "}\n"
    "\n",
    "classname", descriptor_->name(),
    "set_default_unknown_fields", UseUnknownFieldSet(descriptor_)
        ? " this.unknownFields ="
        " com.google.protobuf.UnknownFieldSet.getDefaultInstance(); "
        : " this.unknownFields = com.google.protobuf.ByteString.EMPTY;");

  if (UseUnknownFieldSet(descriptor_)) {
    printer->Print(
        "private final com.google.protobuf.UnknownFieldSet unknownFields;\n"
        ""
        "@java.lang.Override\n"
        "public final com.google.protobuf.UnknownFieldSet\n"
        "    getUnknownFields() {\n"
        "  return this.unknownFields;\n"
        "}\n");
  } else {
    printer->Print(
        "private final com.google.protobuf.ByteString unknownFields;\n");
  }

  if (HasGeneratedMethods(descriptor_)) {
    GenerateParsingConstructor(printer);
  }

  GenerateDescriptorMethods(printer);
  GenerateParser(printer);

  // Nested types
  for (int i = 0; i < descriptor_->enum_type_count(); i++) {
    EnumGenerator(descriptor_->enum_type(i), true, context_)
        .Generate(printer);
  }

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    ImmutableMessageGenerator messageGenerator(
        descriptor_->nested_type(i), context_);
    messageGenerator.GenerateInterface(printer);
    messageGenerator.Generate(printer);
  }

  if (GenerateHasBits(descriptor_)) {
    // Integers for bit fields.
    int totalBits = 0;
    for (int i = 0; i < descriptor_->field_count(); i++) {
    totalBits += field_generators_.get(descriptor_->field(i))
        .GetNumBitsForMessage();
    }
    int totalInts = (totalBits + 31) / 32;
    for (int i = 0; i < totalInts; i++) {
    printer->Print("private int $bit_field_name$;\n",
        "bit_field_name", GetBitFieldName(i));
    }
  }

  // oneof
  map<string, string> vars;
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    vars["oneof_name"] = context_->GetOneofGeneratorInfo(
        descriptor_->oneof_decl(i))->name;
    vars["oneof_capitalized_name"] = context_->GetOneofGeneratorInfo(
        descriptor_->oneof_decl(i))->capitalized_name;
    vars["oneof_index"] = SimpleItoa(descriptor_->oneof_decl(i)->index());
    // oneofCase_ and oneof_
    printer->Print(vars,
    "private int $oneof_name$Case_ = 0;\n"
    "private java.lang.Object $oneof_name$_;\n");
    // OneofCase enum
    printer->Print(vars,
    "public enum $oneof_capitalized_name$Case\n"
    "    implements com.google.protobuf.Internal.EnumLite {\n");
    printer->Indent();
    for (int j = 0; j < descriptor_->oneof_decl(i)->field_count(); j++) {
    const FieldDescriptor* field = descriptor_->oneof_decl(i)->field(j);
    printer->Print(
        "$field_name$($field_number$),\n",
        "field_name",
        ToUpper(field->name()),
        "field_number",
        SimpleItoa(field->number()));
    }
    printer->Print(
    "$cap_oneof_name$_NOT_SET(0);\n",
    "cap_oneof_name",
    ToUpper(vars["oneof_name"]));
    printer->Print(vars,
    "private int value = 0;\n"
    "private $oneof_capitalized_name$Case(int value) {\n"
    "  this.value = value;\n"
    "}\n");
    printer->Print(vars,
    "public static $oneof_capitalized_name$Case valueOf(int value) {\n"
    "  switch (value) {\n");
    for (int j = 0; j < descriptor_->oneof_decl(i)->field_count(); j++) {
    const FieldDescriptor* field = descriptor_->oneof_decl(i)->field(j);
    printer->Print(
        "    case $field_number$: return $field_name$;\n",
        "field_number",
        SimpleItoa(field->number()),
        "field_name",
        ToUpper(field->name()));
    }
    printer->Print(
    "    case 0: return $cap_oneof_name$_NOT_SET;\n"
    "    default: throw new java.lang.IllegalArgumentException(\n"
    "      \"Value is undefined for this oneof enum.\");\n"
    "  }\n"
    "}\n"
    "public int getNumber() {\n"
    "  return this.value;\n"
    "}\n",
    "cap_oneof_name", ToUpper(vars["oneof_name"]));
    printer->Outdent();
    printer->Print("};\n\n");
    // oneofCase()
    printer->Print(vars,
    "public $oneof_capitalized_name$Case\n"
    "get$oneof_capitalized_name$Case() {\n"
    "  return $oneof_capitalized_name$Case.valueOf(\n"
    "      $oneof_name$Case_);\n"
    "}\n"
    "\n");
  }

  // Fields
  for (int i = 0; i < descriptor_->field_count(); i++) {
    printer->Print("public static final int $constant_name$ = $number$;\n",
    "constant_name", FieldConstantName(descriptor_->field(i)),
    "number", SimpleItoa(descriptor_->field(i)->number()));
    field_generators_.get(descriptor_->field(i)).GenerateMembers(printer);
    printer->Print("\n");
  }

  // Called by the constructor, except in the case of the default instance,
  // in which case this is called by static init code later on.
  printer->Print("private void initFields() {\n");
  printer->Indent();
  for (int i = 0; i < descriptor_->field_count(); i++) {
    if (!descriptor_->field(i)->containing_oneof()) {
    field_generators_.get(descriptor_->field(i))
        .GenerateInitializationCode(printer);
    }
  }

  printer->Outdent();
  printer->Print("}\n");

  if (HasGeneratedMethods(descriptor_)) {
    GenerateIsInitialized(printer, MEMOIZE);
    GenerateMessageSerializationMethods(printer);
  }

  if (HasEqualsAndHashCode(descriptor_)) {
    GenerateEqualsAndHashCode(printer);
  }


  GenerateParseFromMethods(printer);
  GenerateBuilder(printer);

  // Carefully initialize the default instance in such a way that it doesn't
  // conflict with other initialization.
  printer->Print(
    "\n"
    "static {\n"
    "  defaultInstance = new $classname$(true);\n"
    "  defaultInstance.initFields();\n"
    "}\n"
    "\n"
    "// @@protoc_insertion_point(class_scope:$full_name$)\n",
    "classname", descriptor_->name(),
    "full_name", descriptor_->full_name());

  // Extensions must be declared after the defaultInstance is initialized
  // because the defaultInstance is used by the extension to lazily retrieve
  // the outer class's FileDescriptor.
  for (int i = 0; i < descriptor_->extension_count(); i++) {
    ImmutableExtensionGenerator(descriptor_->extension(i), context_)
        .Generate(printer);
  }

  printer->Outdent();
  printer->Print("}\n\n");
}


// ===================================================================

void ImmutableMessageGenerator::
GenerateMessageSerializationMethods(io::Printer* printer) {
  scoped_array<const FieldDescriptor*> sorted_fields(
    SortFieldsByNumber(descriptor_));

  vector<const Descriptor::ExtensionRange*> sorted_extensions;
  for (int i = 0; i < descriptor_->extension_range_count(); ++i) {
    sorted_extensions.push_back(descriptor_->extension_range(i));
  }
  sort(sorted_extensions.begin(), sorted_extensions.end(),
        ExtensionRangeOrdering());

  printer->Print(
    "public void writeTo(com.google.protobuf.CodedOutputStream output)\n"
    "                    throws java.io.IOException {\n");
  printer->Indent();
  // writeTo(CodedOutputStream output) might be invoked without
  // getSerializedSize() ever being called, but we need the memoized
  // sizes in case this message has packed fields. Rather than emit checks for
  // each packed field, just call getSerializedSize() up front for all messages.
  // In most cases, getSerializedSize() will have already been called anyway by
  // one of the wrapper writeTo() methods, making this call cheap.
  printer->Print(
    "getSerializedSize();\n");

  if (descriptor_->extension_range_count() > 0) {
    if (descriptor_->options().message_set_wire_format()) {
    printer->Print(
        "com.google.protobuf.GeneratedMessage$lite$\n"
        "  .ExtendableMessage<$classname$>.ExtensionWriter extensionWriter =\n"
        "    newMessageSetExtensionWriter();\n",
        "lite", HasDescriptorMethods(descriptor_) ? "" : "Lite",
        "classname", name_resolver_->GetImmutableClassName(descriptor_));
    } else {
    printer->Print(
        "com.google.protobuf.GeneratedMessage$lite$\n"
        "  .ExtendableMessage<$classname$>.ExtensionWriter extensionWriter =\n"
        "    newExtensionWriter();\n",
        "lite", HasDescriptorMethods(descriptor_) ? "" : "Lite",
        "classname", name_resolver_->GetImmutableClassName(descriptor_));
    }
  }

  // Merge the fields and the extension ranges, both sorted by field number.
  for (int i = 0, j = 0;
        i < descriptor_->field_count() || j < sorted_extensions.size();
        ) {
    if (i == descriptor_->field_count()) {
    GenerateSerializeOneExtensionRange(printer, sorted_extensions[j++]);
    } else if (j == sorted_extensions.size()) {
    GenerateSerializeOneField(printer, sorted_fields[i++]);
    } else if (sorted_fields[i]->number() < sorted_extensions[j]->start) {
    GenerateSerializeOneField(printer, sorted_fields[i++]);
    } else {
    GenerateSerializeOneExtensionRange(printer, sorted_extensions[j++]);
    }
  }

  if (UseUnknownFieldSet(descriptor_)) {
    if (descriptor_->options().message_set_wire_format()) {
    printer->Print(
        "getUnknownFields().writeAsMessageSetTo(output);\n");
    } else {
    printer->Print(
        "getUnknownFields().writeTo(output);\n");
    }
  } else {
    printer->Print(
    "output.writeRawBytes(unknownFields);\n");
  }

  printer->Outdent();
  printer->Print(
    "}\n"
    "\n"
    "private int memoizedSerializedSize = -1;\n"
    "public int getSerializedSize() {\n"
    "  int size = memoizedSerializedSize;\n"
    "  if (size != -1) return size;\n"
    "\n"
    "  size = 0;\n");
  printer->Indent();

  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(sorted_fields[i]).GenerateSerializedSizeCode(printer);
  }

  if (descriptor_->extension_range_count() > 0) {
    if (descriptor_->options().message_set_wire_format()) {
    printer->Print(
        "size += extensionsSerializedSizeAsMessageSet();\n");
    } else {
    printer->Print(
        "size += extensionsSerializedSize();\n");
    }
  }

  if (UseUnknownFieldSet(descriptor_)) {
    if (descriptor_->options().message_set_wire_format()) {
    printer->Print(
        "size += getUnknownFields().getSerializedSizeAsMessageSet();\n");
    } else {
    printer->Print(
        "size += getUnknownFields().getSerializedSize();\n");
    }
  } else {
    printer->Print(
    "size += unknownFields.size();\n");
  }

  printer->Outdent();
  printer->Print(
    "  memoizedSerializedSize = size;\n"
    "  return size;\n"
    "}\n"
    "\n");

  printer->Print(
    "private static final long serialVersionUID = 0L;\n"
    "@java.lang.Override\n"
    "protected java.lang.Object writeReplace()\n"
    "    throws java.io.ObjectStreamException {\n"
    "  return super.writeReplace();\n"
    "}\n"
    "\n");
}

void ImmutableMessageGenerator::
GenerateParseFromMethods(io::Printer* printer) {
  // Note:  These are separate from GenerateMessageSerializationMethods()
  //   because they need to be generated even for messages that are optimized
  //   for code size.
  printer->Print(
    "public static $classname$ parseFrom(\n"
    "    com.google.protobuf.ByteString data)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n"
    "  return PARSER.parseFrom(data);\n"
    "}\n"
    "public static $classname$ parseFrom(\n"
    "    com.google.protobuf.ByteString data,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n"
    "  return PARSER.parseFrom(data, extensionRegistry);\n"
    "}\n"
    "public static $classname$ parseFrom(byte[] data)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n"
    "  return PARSER.parseFrom(data);\n"
    "}\n"
    "public static $classname$ parseFrom(\n"
    "    byte[] data,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n"
    "  return PARSER.parseFrom(data, extensionRegistry);\n"
    "}\n"
    "public static $classname$ parseFrom(java.io.InputStream input)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseFrom(input);\n"
    "}\n"
    "public static $classname$ parseFrom(\n"
    "    java.io.InputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseFrom(input, extensionRegistry);\n"
    "}\n"
    "public static $classname$ parseDelimitedFrom(java.io.InputStream input)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseDelimitedFrom(input);\n"
    "}\n"
    "public static $classname$ parseDelimitedFrom(\n"
    "    java.io.InputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseDelimitedFrom(input, extensionRegistry);\n"
    "}\n"
    "public static $classname$ parseFrom(\n"
    "    com.google.protobuf.CodedInputStream input)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseFrom(input);\n"
    "}\n"
    "public static $classname$ parseFrom(\n"
    "    com.google.protobuf.CodedInputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws java.io.IOException {\n"
    "  return PARSER.parseFrom(input, extensionRegistry);\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
}

void ImmutableMessageGenerator::GenerateSerializeOneField(
    io::Printer* printer, const FieldDescriptor* field) {
  field_generators_.get(field).GenerateSerializationCode(printer);
}

void ImmutableMessageGenerator::GenerateSerializeOneExtensionRange(
    io::Printer* printer, const Descriptor::ExtensionRange* range) {
  printer->Print(
    "extensionWriter.writeUntil($end$, output);\n",
    "end", SimpleItoa(range->end));
}

// ===================================================================

void ImmutableMessageGenerator::GenerateBuilder(io::Printer* printer) {
  printer->Print(
    "public static Builder newBuilder() { return Builder.create(); }\n"
    "public Builder newBuilderForType() { return newBuilder(); }\n"
    "public static Builder newBuilder($classname$ prototype) {\n"
    "  return newBuilder().mergeFrom(prototype);\n"
    "}\n"
    "public Builder toBuilder() { return newBuilder(this); }\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  if (HasNestedBuilders(descriptor_)) {
    printer->Print(
    "@java.lang.Override\n"
    "protected Builder newBuilderForType(\n"
    "    com.google.protobuf.GeneratedMessage.BuilderParent parent) {\n"
    "  Builder builder = new Builder(parent);\n"
    "  return builder;\n"
    "}\n");
  }

  WriteMessageDocComment(printer, descriptor_);

  if (descriptor_->extension_range_count() > 0) {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public static final class Builder extends\n"
        "    com.google.protobuf.GeneratedMessage.ExtendableBuilder<\n"
        "      $classname$, Builder> implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "classname", name_resolver_->GetImmutableClassName(descriptor_),
        "extra_interfaces", ExtraBuilderInterfaces(descriptor_));
    } else {
    printer->Print(
        "public static final class Builder extends\n"
        "    com.google.protobuf.GeneratedMessageLite.ExtendableBuilder<\n"
        "      $classname$, Builder> implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "classname", name_resolver_->GetImmutableClassName(descriptor_),
        "extra_interfaces", ExtraBuilderInterfaces(descriptor_));
    }
  } else {
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public static final class Builder extends\n"
        "    com.google.protobuf.GeneratedMessage.Builder<Builder> implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "classname", name_resolver_->GetImmutableClassName(descriptor_),
        "extra_interfaces", ExtraBuilderInterfaces(descriptor_));
    } else {
    printer->Print(
        "public static final class Builder extends\n"
        "    com.google.protobuf.GeneratedMessageLite.Builder<\n"
        "      $classname$, Builder>\n"
        "    implements\n"
        "    $extra_interfaces$\n"
        "    $classname$OrBuilder {\n",
        "classname", name_resolver_->GetImmutableClassName(descriptor_),
        "extra_interfaces", ExtraBuilderInterfaces(descriptor_));
    }
  }
  printer->Indent();

  GenerateDescriptorMethods(printer);
  GenerateCommonBuilderMethods(printer);

  if (HasGeneratedMethods(descriptor_)) {
    GenerateIsInitialized(printer, DONT_MEMOIZE);
    GenerateBuilderParsingMethods(printer);
  }

  // oneof
  map<string, string> vars;
  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    vars["oneof_name"] = context_->GetOneofGeneratorInfo(
        descriptor_->oneof_decl(i))->name;
    vars["oneof_capitalized_name"] = context_->GetOneofGeneratorInfo(
        descriptor_->oneof_decl(i))->capitalized_name;
    vars["oneof_index"] = SimpleItoa(descriptor_->oneof_decl(i)->index());
    // oneofCase_ and oneof_
    printer->Print(vars,
    "private int $oneof_name$Case_ = 0;\n"
    "private java.lang.Object $oneof_name$_;\n");
    // oneofCase() and clearOneof()
    printer->Print(vars,
    "public $oneof_capitalized_name$Case\n"
    "    get$oneof_capitalized_name$Case() {\n"
    "  return $oneof_capitalized_name$Case.valueOf(\n"
    "      $oneof_name$Case_);\n"
    "}\n"
    "\n"
    "public Builder clear$oneof_capitalized_name$() {\n"
    "  $oneof_name$Case_ = 0;\n"
    "  $oneof_name$_ = null;\n");
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print("  onChanged();\n");
    }
    printer->Print(
    "  return this;\n"
    "}\n"
    "\n");
  }

  if (GenerateHasBits(descriptor_)) {
    // Integers for bit fields.
    int totalBits = 0;
    for (int i = 0; i < descriptor_->field_count(); i++) {
    totalBits += field_generators_.get(descriptor_->field(i))
        .GetNumBitsForBuilder();
    }
    int totalInts = (totalBits + 31) / 32;
    for (int i = 0; i < totalInts; i++) {
    printer->Print("private int $bit_field_name$;\n",
        "bit_field_name", GetBitFieldName(i));
    }
  }

  for (int i = 0; i < descriptor_->field_count(); i++) {
    printer->Print("\n");
    field_generators_.get(descriptor_->field(i))
                    .GenerateBuilderMembers(printer);
  }

  printer->Print(
    "\n"
    "// @@protoc_insertion_point(builder_scope:$full_name$)\n",
    "full_name", descriptor_->full_name());

  printer->Outdent();
  printer->Print("}\n");
}

void ImmutableMessageGenerator::
GenerateDescriptorMethods(io::Printer* printer) {
  if (HasDescriptorMethods(descriptor_)) {
    if (!descriptor_->options().no_standard_descriptor_accessor()) {
    printer->Print(
        "public static final com.google.protobuf.Descriptors.Descriptor\n"
        "    getDescriptor() {\n"
        "  return $fileclass$.internal_$identifier$_descriptor;\n"
        "}\n"
        "\n",
        "fileclass", name_resolver_->GetImmutableClassName(descriptor_->file()),
        "identifier", UniqueFileScopeIdentifier(descriptor_));
    }
    printer->Print(
    "protected com.google.protobuf.GeneratedMessage.FieldAccessorTable\n"
    "    internalGetFieldAccessorTable() {\n"
    "  return $fileclass$.internal_$identifier$_fieldAccessorTable\n"
    "      .ensureFieldAccessorsInitialized(\n"
    "          $classname$.class, $classname$.Builder.class);\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_),
    "fileclass", name_resolver_->GetImmutableClassName(descriptor_->file()),
    "identifier", UniqueFileScopeIdentifier(descriptor_));
  }
}

// ===================================================================

void ImmutableMessageGenerator::
GenerateCommonBuilderMethods(io::Printer* printer) {
  printer->Print(
    "// Construct using $classname$.newBuilder()\n"
    "private Builder() {\n"
    "  maybeForceBuilderInitialization();\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
    "private Builder(\n"
    "    com.google.protobuf.GeneratedMessage.BuilderParent parent) {\n"
    "  super(parent);\n"
    "  maybeForceBuilderInitialization();\n"
    "}\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
  }


  if (HasNestedBuilders(descriptor_)) {
    printer->Print(
    "private void maybeForceBuilderInitialization() {\n"
    "  if (com.google.protobuf.GeneratedMessage.alwaysUseFieldBuilders) {\n");

    printer->Indent();
    printer->Indent();
    for (int i = 0; i < descriptor_->field_count(); i++) {
    if (!descriptor_->field(i)->containing_oneof()) {
        field_generators_.get(descriptor_->field(i))
            .GenerateFieldBuilderInitializationCode(printer);
    }
    }
    printer->Outdent();
    printer->Outdent();

    printer->Print(
    "  }\n"
    "}\n");
  } else {
    printer->Print(
    "private void maybeForceBuilderInitialization() {\n"
    "}\n");
  }

  printer->Print(
    "private static Builder create() {\n"
    "  return new Builder();\n"
    "}\n"
    "\n"
    "public Builder clear() {\n"
    "  super.clear();\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  printer->Indent();

  for (int i = 0; i < descriptor_->field_count(); i++) {
    if (!descriptor_->field(i)->containing_oneof()) {
    field_generators_.get(descriptor_->field(i))
        .GenerateBuilderClearCode(printer);
    }
  }

  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    printer->Print(
    "$oneof_name$Case_ = 0;\n"
    "$oneof_name$_ = null;\n",
    "oneof_name", context_->GetOneofGeneratorInfo(
        descriptor_->oneof_decl(i))->name);
  }

  printer->Outdent();

  printer->Print(
    "  return this;\n"
    "}\n"
    "\n"
    "public Builder clone() {\n"
    "  return create().mergeFrom(buildPartial());\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
  if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
    "public com.google.protobuf.Descriptors.Descriptor\n"
    "    getDescriptorForType() {\n"
    "  return $fileclass$.internal_$identifier$_descriptor;\n"
    "}\n"
    "\n",
    "fileclass", name_resolver_->GetImmutableClassName(descriptor_->file()),
    "identifier", UniqueFileScopeIdentifier(descriptor_));
  }
  printer->Print(
    "public $classname$ getDefaultInstanceForType() {\n"
    "  return $classname$.getDefaultInstance();\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  // -----------------------------------------------------------------

  printer->Print(
    "public $classname$ build() {\n"
    "  $classname$ result = buildPartial();\n"
    "  if (!result.isInitialized()) {\n"
    "    throw newUninitializedMessageException(result);\n"
    "  }\n"
    "  return result;\n"
    "}\n"
    "\n"
    "public $classname$ buildPartial() {\n"
    "  $classname$ result = new $classname$(this);\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  printer->Indent();

  int totalBuilderBits = 0;
  int totalMessageBits = 0;
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const ImmutableFieldGenerator& field =
        field_generators_.get(descriptor_->field(i));
    totalBuilderBits += field.GetNumBitsForBuilder();
    totalMessageBits += field.GetNumBitsForMessage();
  }
  int totalBuilderInts = (totalBuilderBits + 31) / 32;
  int totalMessageInts = (totalMessageBits + 31) / 32;

  if (GenerateHasBits(descriptor_)) {
    // Local vars for from and to bit fields to avoid accessing the builder and
    // message over and over for these fields. Seems to provide a slight
    // perforamance improvement in micro benchmark and this is also what proto1
    // code does.
    for (int i = 0; i < totalBuilderInts; i++) {
    printer->Print("int from_$bit_field_name$ = $bit_field_name$;\n",
        "bit_field_name", GetBitFieldName(i));
    }
    for (int i = 0; i < totalMessageInts; i++) {
    printer->Print("int to_$bit_field_name$ = 0;\n",
        "bit_field_name", GetBitFieldName(i));
    }
  }

  // Output generation code for each field.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    field_generators_.get(descriptor_->field(i)).GenerateBuildingCode(printer);
  }

  if (GenerateHasBits(descriptor_)) {
    // Copy the bit field results to the generated message
    for (int i = 0; i < totalMessageInts; i++) {
    printer->Print("result.$bit_field_name$ = to_$bit_field_name$;\n",
        "bit_field_name", GetBitFieldName(i));
    }
  }

  for (int i = 0; i < descriptor_->oneof_decl_count(); i++) {
    printer->Print("result.$oneof_name$Case_ = $oneof_name$Case_;\n",
                    "oneof_name", context_->GetOneofGeneratorInfo(
                        descriptor_->oneof_decl(i))->name);
  }

  printer->Outdent();

  if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
    "  onBuilt();\n");
  }

  printer->Print(
    "  return result;\n"
    "}\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  // -----------------------------------------------------------------

  if (HasGeneratedMethods(descriptor_)) {
    // MergeFrom(Message other) requires the ability to distinguish the other
    // messages type by its descriptor.
    if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
        "public Builder mergeFrom(com.google.protobuf.Message other) {\n"
        "  if (other instanceof $classname$) {\n"
        "    return mergeFrom(($classname$)other);\n"
        "  } else {\n"
        "    super.mergeFrom(other);\n"
        "    return this;\n"
        "  }\n"
        "}\n"
        "\n",
        "classname", name_resolver_->GetImmutableClassName(descriptor_));
    }

    printer->Print(
    "public Builder mergeFrom($classname$ other) {\n"
    // Optimization:  If other is the default instance, we know none of its
    //   fields are set so we can skip the merge.
    "  if (other == $classname$.getDefaultInstance()) return this;\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
    printer->Indent();

    for (int i = 0; i < descriptor_->field_count(); i++) {
    if (!descriptor_->field(i)->containing_oneof()) {
        field_generators_.get(
            descriptor_->field(i)).GenerateMergingCode(printer);
    }
    }

    // Merge oneof fields.
    for (int i = 0; i < descriptor_->oneof_decl_count(); ++i) {
    printer->Print(
        "switch (other.get$oneof_capitalized_name$Case()) {\n",
        "oneof_capitalized_name",
        context_->GetOneofGeneratorInfo(
            descriptor_->oneof_decl(i))->capitalized_name);
    printer->Indent();
    for (int j = 0; j < descriptor_->oneof_decl(i)->field_count(); j++) {
        const FieldDescriptor* field = descriptor_->oneof_decl(i)->field(j);
        printer->Print(
        "case $field_name$: {\n",
        "field_name",
        ToUpper(field->name()));
        printer->Indent();
        field_generators_.get(field).GenerateMergingCode(printer);
        printer->Print(
            "break;\n");
        printer->Outdent();
        printer->Print(
            "}\n");
    }
    printer->Print(
        "case $cap_oneof_name$_NOT_SET: {\n"
        "  break;\n"
        "}\n",
        "cap_oneof_name",
        ToUpper(context_->GetOneofGeneratorInfo(
            descriptor_->oneof_decl(i))->name));
    printer->Outdent();
    printer->Print(
        "}\n");
    }

    printer->Outdent();

    // if message type has extensions
    if (descriptor_->extension_range_count() > 0) {
    printer->Print(
        "  this.mergeExtensionFields(other);\n");
    }

    if (UseUnknownFieldSet(descriptor_)) {
    printer->Print(
        "  this.mergeUnknownFields(other.getUnknownFields());\n");
    } else {
    printer->Print(
        "  setUnknownFields(\n"
        "      getUnknownFields().concat(other.unknownFields));\n");
    }

    printer->Print(
    "  return this;\n"
    "}\n"
    "\n");
  }
}

// ===================================================================

void ImmutableMessageGenerator::
GenerateBuilderParsingMethods(io::Printer* printer) {
  printer->Print(
    "public Builder mergeFrom(\n"
    "    com.google.protobuf.CodedInputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws java.io.IOException {\n"
    "  $classname$ parsedMessage = null;\n"
    "  try {\n"
    "    parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);\n"
    "  } catch (com.google.protobuf.InvalidProtocolBufferException e) {\n"
    "    parsedMessage = ($classname$) e.getUnfinishedMessage();\n"
    "    throw e;\n"
    "  } finally {\n"
    "    if (parsedMessage != null) {\n"
    "      mergeFrom(parsedMessage);\n"
    "    }\n"
    "  }\n"
    "  return this;\n"
    "}\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
}

// ===================================================================

void ImmutableMessageGenerator::GenerateIsInitialized(
    io::Printer* printer, UseMemoization useMemoization) {
  bool memoization = useMemoization == MEMOIZE;
  if (memoization) {
    // Memoizes whether the protocol buffer is fully initialized (has all
    // required fields). -1 means not yet computed. 0 means false and 1 means
    // true.
    printer->Print(
    "private byte memoizedIsInitialized = -1;\n");
  }
  printer->Print(
    "public final boolean isInitialized() {\n");
  printer->Indent();

  if (memoization) {
    // Don't directly compare to -1 to avoid an Android x86 JIT bug.
    printer->Print(
    "byte isInitialized = memoizedIsInitialized;\n"
    "if (isInitialized == 1) return true;\n"
    "if (isInitialized == 0) return false;\n"
    "\n");
  }

  // Check that all required fields in this message are set.
  // TODO(kenton):  We can optimize this when we switch to putting all the
  //   "has" fields into a single bitfield.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    const FieldGeneratorInfo* info = context_->GetFieldGeneratorInfo(field);

    if (field->is_required()) {
    printer->Print(
        "if (!has$name$()) {\n"
        "  $memoize$\n"
        "  return false;\n"
        "}\n",
        "name", info->capitalized_name,
        "memoize", memoization ? "memoizedIsInitialized = 0;" : "");
    }
  }

  // Now check that all embedded messages are initialized.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    const FieldGeneratorInfo* info = context_->GetFieldGeneratorInfo(field);
    if (GetJavaType(field) == JAVATYPE_MESSAGE &&
        HasRequiredFields(field->message_type())) {
    switch (field->label()) {
        case FieldDescriptor::LABEL_REQUIRED:
        printer->Print(
            "if (!get$name$().isInitialized()) {\n"
            "  $memoize$\n"
            "  return false;\n"
            "}\n",
            "type", name_resolver_->GetImmutableClassName(
                field->message_type()),
            "name", info->capitalized_name,
            "memoize", memoization ? "memoizedIsInitialized = 0;" : "");
        break;
        case FieldDescriptor::LABEL_OPTIONAL:
        printer->Print(
            "if (has$name$()) {\n"
            "  if (!get$name$().isInitialized()) {\n"
            "    $memoize$\n"
            "    return false;\n"
            "  }\n"
            "}\n",
            "type", name_resolver_->GetImmutableClassName(
                field->message_type()),
            "name", info->capitalized_name,
            "memoize", memoization ? "memoizedIsInitialized = 0;" : "");
        break;
        case FieldDescriptor::LABEL_REPEATED:
        printer->Print(
            "for (int i = 0; i < get$name$Count(); i++) {\n"
            "  if (!get$name$(i).isInitialized()) {\n"
            "    $memoize$\n"
            "    return false;\n"
            "  }\n"
            "}\n",
            "type", name_resolver_->GetImmutableClassName(
                field->message_type()),
            "name", info->capitalized_name,
            "memoize", memoization ? "memoizedIsInitialized = 0;" : "");
        break;
    }
    }
  }

  if (descriptor_->extension_range_count() > 0) {
    printer->Print(
    "if (!extensionsAreInitialized()) {\n"
    "  $memoize$\n"
    "  return false;\n"
    "}\n",
    "memoize", memoization ? "memoizedIsInitialized = 0;" : "");
  }

  printer->Outdent();

  if (memoization) {
    printer->Print(
    "  memoizedIsInitialized = 1;\n");
  }

  printer->Print(
    "  return true;\n"
    "}\n"
    "\n");
}

// ===================================================================

namespace {
bool CheckHasBitsForEqualsAndHashCode(const FieldDescriptor* field) {
  if (field->is_repeated()) {
    return false;
  }
  if (SupportFieldPresence(field->file())) {
    return true;
  }
  return GetJavaType(field) == JAVATYPE_MESSAGE &&
    field->containing_oneof() == NULL;
}
}  // namespace

void ImmutableMessageGenerator::
GenerateEqualsAndHashCode(io::Printer* printer) {
  printer->Print(
    "@java.lang.Override\n"
    "public boolean equals(final java.lang.Object obj) {\n");
  printer->Indent();
  printer->Print(
    "if (obj == this) {\n"
    " return true;\n"
    "}\n"
    "if (!(obj instanceof $classname$)) {\n"
    "  return super.equals(obj);\n"
    "}\n"
    "$classname$ other = ($classname$) obj;\n"
    "\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));

  printer->Print("boolean result = true;\n");
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    const FieldGeneratorInfo* info = context_->GetFieldGeneratorInfo(field);
    bool check_has_bits = CheckHasBitsForEqualsAndHashCode(field);
    if (check_has_bits) {
    printer->Print(
        "result = result && (has$name$() == other.has$name$());\n"
        "if (has$name$()) {\n",
        "name", info->capitalized_name);
    printer->Indent();
    }
    field_generators_.get(field).GenerateEqualsCode(printer);
    if (check_has_bits) {
    printer->Outdent();
    printer->Print(
        "}\n");
    }
  }
  if (HasDescriptorMethods(descriptor_)) {
    printer->Print(
    "result = result &&\n"
    "    getUnknownFields().equals(other.getUnknownFields());\n");
    if (descriptor_->extension_range_count() > 0) {
    printer->Print(
        "result = result &&\n"
        "    getExtensionFields().equals(other.getExtensionFields());\n");
    }
  }
  printer->Print(
    "return result;\n");
  printer->Outdent();
  printer->Print(
    "}\n"
    "\n");

  printer->Print(
    "@java.lang.Override\n"
    "public int hashCode() {\n");
  printer->Indent();
  printer->Print(
    "if (memoizedHashCode != 0) {\n");
  printer->Indent();
  printer->Print(
    "return memoizedHashCode;\n");
  printer->Outdent();
  printer->Print(
    "}\n"
    "int hash = 41;\n");

  if (HasDescriptorMethods(descriptor_)) {
    printer->Print("hash = (19 * hash) + getDescriptorForType().hashCode();\n");
  } else {
    // Include the hash of the class so that two objects with different types
    // but the same field values will probably have different hashes.
    printer->Print("hash = (19 * hash) + $classname$.class.hashCode();\n",
    "classname", name_resolver_->GetImmutableClassName(descriptor_));
  }

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = descriptor_->field(i);
    const FieldGeneratorInfo* info = context_->GetFieldGeneratorInfo(field);
    bool check_has_bits = CheckHasBitsForEqualsAndHashCode(field);
    if (check_has_bits) {
    printer->Print(
        "if (has$name$()) {\n",
        "name", info->capitalized_name);
    printer->Indent();
    }
    field_generators_.get(field).GenerateHashCode(printer);
    if (check_has_bits) {
    printer->Outdent();
    printer->Print("}\n");
    }
  }
  if (HasDescriptorMethods(descriptor_)) {
    if (descriptor_->extension_range_count() > 0) {
    printer->Print(
        "hash = hashFields(hash, getExtensionFields());\n");
    }
  }

  if (UseUnknownFieldSet(descriptor_)) {
    printer->Print(
    "hash = (29 * hash) + getUnknownFields().hashCode();\n");
  } else {
    printer->Print(
    "hash = (29 * hash) + unknownFields.hashCode();\n");
  }
  printer->Print(
    "memoizedHashCode = hash;\n"
    "return hash;\n");
  printer->Outdent();
  printer->Print(
    "}\n"
    "\n");
}

// ===================================================================

void ImmutableMessageGenerator::
GenerateExtensionRegistrationCode(io::Printer* printer) {
  for (int i = 0; i < descriptor_->extension_count(); i++) {
    ImmutableExtensionGenerator(descriptor_->extension(i), context_)
    .GenerateRegistrationCode(printer);
  }

  for (int i = 0; i < descriptor_->nested_type_count(); i++) {
    ImmutableMessageGenerator(descriptor_->nested_type(i), context_)
    .GenerateExtensionRegistrationCode(printer);
  }
}

// ===================================================================
void ImmutableMessageGenerator::
GenerateParsingConstructor(io::Printer* printer) {
  scoped_array<const FieldDescriptor*> sorted_fields(
    SortFieldsByNumber(descriptor_));

  printer->Print(
    "private $classname$(\n"
    "    com.google.protobuf.CodedInputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n",
    "classname", descriptor_->name());
  printer->Indent();

  // Initialize all fields to default.
  printer->Print(
    "initFields();\n");

  // Use builder bits to track mutable repeated fields.
  int totalBuilderBits = 0;
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const ImmutableFieldGenerator& field =
        field_generators_.get(descriptor_->field(i));
    totalBuilderBits += field.GetNumBitsForBuilder();
  }
  int totalBuilderInts = (totalBuilderBits + 31) / 32;
  for (int i = 0; i < totalBuilderInts; i++) {
    printer->Print("int mutable_$bit_field_name$ = 0;\n",
    "bit_field_name", GetBitFieldName(i));
  }

  if (UseUnknownFieldSet(descriptor_)) {
    printer->Print(
    "com.google.protobuf.UnknownFieldSet.Builder unknownFields =\n"
    "    com.google.protobuf.UnknownFieldSet.newBuilder();\n");
  } else {
    printer->Print(
    "com.google.protobuf.ByteString.Output unknownFieldsOutput =\n"
    "    com.google.protobuf.ByteString.newOutput();\n"
    "com.google.protobuf.CodedOutputStream unknownFieldsCodedOutput =\n"
    "    com.google.protobuf.CodedOutputStream.newInstance(\n"
    "        unknownFieldsOutput);\n");
  }

  printer->Print(
    "try {\n");
  printer->Indent();

  printer->Print(
    "boolean done = false;\n"
    "while (!done) {\n");
  printer->Indent();

  printer->Print(
    "int tag = input.readTag();\n"
    "switch (tag) {\n");
  printer->Indent();

  printer->Print(
    "case 0:\n"          // zero signals EOF / limit reached
    "  done = true;\n"
    "  break;\n"
    "default: {\n"
    "  if (!parseUnknownField(input,$unknown_fields$\n"
    "                         extensionRegistry, tag)) {\n"
    "    done = true;\n"  // it's an endgroup tag
    "  }\n"
    "  break;\n"
    "}\n",
    "unknown_fields", UseUnknownFieldSet(descriptor_)
        ? " unknownFields," : " unknownFieldsCodedOutput,");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = sorted_fields[i];
    uint32 tag = WireFormatLite::MakeTag(field->number(),
    WireFormat::WireTypeForFieldType(field->type()));

    printer->Print(
    "case $tag$: {\n",
    "tag", SimpleItoa(tag));
    printer->Indent();

    field_generators_.get(field).GenerateParsingCode(printer);

    printer->Outdent();
    printer->Print(
    "  break;\n"
    "}\n");

    if (field->is_packable()) {
    // To make packed = true wire compatible, we generate parsing code from a
    // packed version of this field regardless of field->options().packed().
    uint32 packed_tag = WireFormatLite::MakeTag(field->number(),
        WireFormatLite::WIRETYPE_LENGTH_DELIMITED);
    printer->Print(
        "case $tag$: {\n",
        "tag", SimpleItoa(packed_tag));
    printer->Indent();

    field_generators_.get(field).GenerateParsingCodeFromPacked(printer);

    printer->Outdent();
    printer->Print(
        "  break;\n"
        "}\n");
    }
  }

  printer->Outdent();
  printer->Outdent();
  printer->Print(
    "  }\n"     // switch (tag)
    "}\n");     // while (!done)

  printer->Outdent();
  printer->Print(
    "} catch (com.google.protobuf.InvalidProtocolBufferException e) {\n"
    "  throw e.setUnfinishedMessage(this);\n"
    "} catch (java.io.IOException e) {\n"
    "  throw new com.google.protobuf.InvalidProtocolBufferException(\n"
    "      e.getMessage()).setUnfinishedMessage(this);\n"
    "} finally {\n");
  printer->Indent();

  // Make repeated field list immutable.
  for (int i = 0; i < descriptor_->field_count(); i++) {
    const FieldDescriptor* field = sorted_fields[i];
    field_generators_.get(field).GenerateParsingDoneCode(printer);
  }

  // Make unknown fields immutable.
  if (UseUnknownFieldSet(descriptor_)) {
    printer->Print(
        "this.unknownFields = unknownFields.build();\n");
  } else {
    printer->Print(
        "try {\n"
        "  unknownFieldsCodedOutput.flush();\n"
        "} catch (java.io.IOException e) {\n"
        "// Should not happen\n"
        "} finally {\n"
        "  unknownFields = unknownFieldsOutput.toByteString();\n"
        "}\n");
  }

  // Make extensions immutable.
  printer->Print(
    "makeExtensionsImmutable();\n");

  printer->Outdent();
  printer->Outdent();
  printer->Print(
    "  }\n"     // finally
    "}\n");
}

// ===================================================================
void ImmutableMessageGenerator::GenerateParser(io::Printer* printer) {
  printer->Print(
    "public static com.google.protobuf.Parser<$classname$> PARSER =\n"
    "    new com.google.protobuf.AbstractParser<$classname$>() {\n",
    "classname", descriptor_->name());
  printer->Indent();
  printer->Print(
    "public $classname$ parsePartialFrom(\n"
    "    com.google.protobuf.CodedInputStream input,\n"
    "    com.google.protobuf.ExtensionRegistryLite extensionRegistry)\n"
    "    throws com.google.protobuf.InvalidProtocolBufferException {\n",
    "classname", descriptor_->name());
  if (HasGeneratedMethods(descriptor_)) {
    printer->Print(
        "  return new $classname$(input, extensionRegistry);\n",
        "classname", descriptor_->name());
  } else {
    // When parsing constructor isn't generated, use builder to parse messages.
    // Note, will fallback to use reflection based mergeFieldFrom() in
    // AbstractMessage.Builder.
    printer->Indent();
    printer->Print(
        "Builder builder = newBuilder();\n"
        "try {\n"
        "  builder.mergeFrom(input, extensionRegistry);\n"
        "} catch (com.google.protobuf.InvalidProtocolBufferException e) {\n"
        "  throw e.setUnfinishedMessage(builder.buildPartial());\n"
        "} catch (java.io.IOException e) {\n"
        "  throw new com.google.protobuf.InvalidProtocolBufferException(\n"
        "      e.getMessage()).setUnfinishedMessage(builder.buildPartial());\n"
        "}\n"
        "return builder.buildPartial();\n");
    printer->Outdent();
  }
  printer->Print(
        "}\n");
  printer->Outdent();
  printer->Print(
    "};\n"
    "\n");

  printer->Print(
    "@java.lang.Override\n"
    "public com.google.protobuf.Parser<$classname$> getParserForType() {\n"
    "  return PARSER;\n"
    "}\n"
    "\n",
    "classname", descriptor_->name());
}


}  // namespace java
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
