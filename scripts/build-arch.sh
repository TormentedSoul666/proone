#!/bin/bash
set -e

MISC_BIN="
	proone-stress
	proone-resolv
	proone-test_proto
	proone-test_util
"
BIN_PATH="$PROONE_EXEC_PREFIX.$PROONE_BIN_OS.$PROONE_BIN_ARCH"
ENTIRE_BIN_PATH="$PROONE_ENTIRE_PREFIX.$PROONE_BIN_OS.$PROONE_BIN_ARCH"
READELF_PATH="$PROONE_READELF_PREFIX.$PROONE_BIN_OS.$PROONE_BIN_ARCH"
ASM_PATH="$PROONE_ASM_PREFIX.$PROONE_BIN_OS.$PROONE_BIN_ARCH"

separate_debug() {
	cp -a "$1" "$2"
	if [ ! -z "$4" ]; then
		cp -a "$1" "$4"
		"$PROONE_HOST"-readelf -a "$4" > "$READELF_PATH"
		# "$PROONE_HOST"-objdump -D "$4" | xz -evvT0 > "$ASM_PATH"
	fi
	"$PROONE_HOST-objcopy" --only-keep-debug "$2" "$3"
	"$PROONE_HOST-strip"\
		-S\
		--strip-unneeded\
		--remove-section=.note.gnu.gold-version\
		--remove-section=.comment\
		--remove-section=.note\
		--remove-section=.note.gnu.build-id\
		--remove-section=.note.ABI-tag\
		--remove-section=.jcr\
		--remove-section=.got.plt\
		--remove-section=.eh_frame\
		--remove-section=.eh_frame_ptr\
		--remove-section=.eh_frame_hdr\
		"$2"
	"$PROONE_HOST-objcopy" --add-gnu-debuglink="$3" "$2"
}

./configure --host="$PROONE_HOST" --enable-static $PROONE_AM_CONF
cd src
make -j$(nproc) proone.bin $MISC_BIN
cd ..

separate_debug\
	src/proone.bin\
	"$BIN_PATH"\
	"$PROONE_DEBUG_SYM_PREFIX""proone.sym.$PROONE_BIN_OS.$PROONE_BIN_ARCH"\
	"$ENTIRE_BIN_PATH"
for b in $MISC_BIN; do
	separate_debug\
		"src/$b"\
		"$PROONE_MISC_BIN_PREFIX/$b.$PROONE_BIN_OS.$PROONE_BIN_ARCH"\
		"$PROONE_DEBUG_SYM_PREFIX""$b.sym.$PROONE_BIN_OS.$PROONE_BIN_ARCH"
done

make distclean
