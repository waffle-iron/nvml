# Copyright 2014-2016, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# src/Makefile.inc -- common Makefile rules for NVM library
#

TOP := $(dir $(lastword $(MAKEFILE_LIST)))..

LN = ln
OBJCOPY = objcopy
MKDIR = mkdir
INSTALL = install
CP = cp
CSTYLE = $(TOP)/utils/cstyle -pP
PKG_CONFIG = pkg-config

ifeq ($(shell command -v $(PKG_CONFIG) && echo y || echo n), n)
$(error $(PKG_CONFIG) not found)
endif

check_package = $(shell $(PKG_CONFIG) $(1) && echo y || echo n)

check_flag = $(shell echo "int main(){return 0;}" |\
	$(CC) $(CFLAGS) $(1) -x c -o /dev/null - 2>/dev/null && echo y || echo n)

check_cxx_flags = $(shell echo "int main(){return 0;}" |\
	$(CXX) $(1) -x c++ -o /dev/null - 2>/dev/null && echo y || echo n)

# This is a workaround for older incompatible versions of libstdc++ and clang.
# Please see https://llvm.org/bugs/show_bug.cgi?id=15517 for more info.
check_cxx_chrono = $(shell echo "int main(){return 0;}" |\
	$(CXX) -std=c++11 -x c++ -include future -o /dev/null - 2>/dev/null && echo y || echo n)

check_Wconversion = $(shell echo "long int random(void); char test(void); char test(void){char a = 0; char b = 'a'; char ret = random() == 1 ? a : b; return ret;}" |\
	$(CC) -c $(CFLAGS) -Wconversion -x c -o /dev/null - 2>/dev/null && echo y || echo n)

check_librt = $(shell echo "int main() { struct timespec t; return clock_gettime(CLOCK_MONOTONIC, &t); }" |\
	$(CC) $(CFLAGS) -x c -include time.h -o /dev/null - 2>/dev/null && echo y || echo n)

define create-deps
	@cp $(objdir)/$*.d $(objdir)/.deps/$*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	    -e '/^$$/ d' -e 's/$$/ :/' < $(objdir)/$*.d >> $(objdir)/.deps/$*.P; \
	$(RM) -f $(objdir)/$*.d
endef

export prefix = /usr/local
export exec_prefix := $(prefix)
export sysconfdir := $(prefix)/etc
export datarootdir := $(prefix)/share
export mandir := $(datarootdir)/man
export man1dir := $(mandir)/man1
export man3dir := $(mandir)/man3

ifneq ($(wildcard $(exec_prefix)/x86_64-linux-gnu),)
LIB_PREFIX ?= x86_64-linux-gnu/lib
endif

ifneq ($(wildcard $(exec_prefix)/lib64),)
LIB_PREFIX ?= lib64
endif

LIB_PREFIX ?= lib

ifneq ($(wildcard $(prefix)/x86_64-linux-gnu),)
INC_PREFIX ?= x86_64-linux-gnu/include
endif

INC_PREFIX ?= include

export libdir := $(exec_prefix)/$(LIB_PREFIX)
export includedir := $(prefix)/$(INC_PREFIX)
export pkgconfigdir := $(libdir)/pkgconfig
export bindir := $(exec_prefix)/bin
export bashcompdir := $(sysconfdir)/bash_completion.d
