SUBDIRS = build

bin_SCRIPTS = openalchemist-config

dist_pkgdata_DATA = skins/aqua skins/brushed skins/vectoriel data/* music/* sounds/* CODE-LICENSE GRAPHICS-LICENSE skins/aqua.zip skins/brushed.zip skins/vectoriel.zip

all : skins/aqua.zip skins/brushed.zip skins/vectoriel.zip
	@echo "-----------------------------------------"
	@echo "Compilation complete. Tip \"make install\" (as root) to finish installation."

skins/aqua.zip : skins/aqua/*
	cd skins/aqua && zip -q -r ../aqua * -x *svn*
skins/brushed.zip : skins/brushed/*
	cd skins/brushed && zip -q -r ../brushed * -x *svn*
skins/vectoriel.zip : skins/vectoriel/*
	cd skins/vectoriel && zip -q -r ../vectoriel * -x *svn*

CLEANFILES = skins/*.zip

