#!/bin/sh

#Descarga las herramientas
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O appimagetool
chmod +x appimagetool

# crea el directorio
mkdir -p Zen-Web.AppDir/usr/bin
mkdir -p Zen-Web.AppDir/usr/share/applications
mkdir -p Zen-Web.AppDir/usr/share/icons/hicolor/128x128/apps

#recursos
cp zen-web Zen-Web.AppDir/usr/bin/
cp -r assets Zen-Web.AppDir/usr/bin/
cp index.html Zen-Web.AppDir/usr/bin/

#icono
cp zen-web.png Zen-Web.AppDir/usr/share/icons/hicolor/128x128/apps/

#Zen-Web.AppDir/usr/share/applications/zen-web.desktop
#[Desktop Entry]
#Name=Zen-Web
#Exec=zen-web
#Icon=zen-web
#Type=Application
#Categories=Network;WebBrowser;

#empaqueta el AppImage
./appimagetool Zen-Web.AppDir

