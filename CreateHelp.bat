#
# Bind HTML pages to context helps. If you modify help texts, you
# don't need recompilation, if you run this script.
# Joni-Matti M��tt�, Aug 2012
#
@echo off
qhelpgenerator Help/kactus2help.qhp -o Help/Kactus2Help.qch
qcollectiongenerator Help/kactus2help.qhcp -o Help/Kactus2Help.qhc
xcopy /Y Help\Kactus2Help.qhc executable\Help\
xcopy /Y Help\Kactus2Help.qch executable\Help\