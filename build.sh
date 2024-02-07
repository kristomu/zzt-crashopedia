#!/bin/sh
rm zztcrash_current.zip
./compile.sh
./splice worlds/ZZTCRASH.ZZT boards/corrupt/ ZZTCRASH.ZZT
zip --junk-paths zztcrash_current.zip worlds/*.ZZT worlds/LICENSE docs/archive/README.TXT
zip zztcrash_current.zip ZZTCRASH.ZZT
