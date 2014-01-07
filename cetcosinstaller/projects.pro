TEMPLATE = subdirs
SUBDIRS = src
RESOURCES += data/svg/svg.qrc

TRANSLATIONS += translations/installer_en.ts \
                translations/installer_es.ts \
                translations/installer_pt.ts \
                translations/installer_zh_CN.ts \
                translations/installer_zh_TW.ts
translations.files = translations/*.qm translations/*.html
translations.path = /usr/share/cetcosinstaller/translations

images.files = data/images/*
images.path = /usr/share/cetcosinstaller/images
svg.files = data/svg/*.svg
svg.path = /usr/share/cetcosinstaller/svg
desktop.files = data/cetcosinstaller.desktop
desktop.path = /usr/share/applications
locale.files = data/rflocale.xml
locale.path = /etc

INSTALLS += translations images svg desktop locale
