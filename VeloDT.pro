TEMPLATE = subdirs
CONFIG -= app_bundle
CONFIG += ordered
SUBDIRS += src/common src/V2RhoT src/V2T
V2RhoT.depends = common
V2T.depends = common
