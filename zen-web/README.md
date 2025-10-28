# 🧘‍♂️ Zen Kiosk

**ES | Español**

Zen Kiosk es un navegador ceremonial y minimalista. Diseñado para microdistribuciones, permite abrir archivos locales, WebApps modernas y ejecutar aplicaciones `.jar` como presencias externas.

## ✨ Características

- Carga archivos locales (`.html`, `.mp4`, etc.)
- Abre WebApps modernas con soporte para:
  - JavaScript y WebAssembly
  - WebRTC y MediaStream
  - IndexedDB y almacenamiento local
  - PWA (Progressive Web Apps)
- Ejecuta archivos `.jar` con `java -jar`
- Modo incógnito por defecto en modo ventana (sin historial ni cookies)
- Interfaz visual opcional con grid de íconos (`zen.html`)
- Selector de buscador configurable (Google, DuckDuckGo, Brave, Ecosia)

## 🛠️ Compilación

```bash
sudo apt install libwebkit2gtk-4.0-dev
make
```

## 🚀 Ejecución

```bash
./zen_kiosk google.com
./zen_kiosk ./archivo.html
./zen_kiosk ./aplicacion.jar
```

## 📁 Estructura recomendada

```
zen_kiosk/
├── src/zen_kiosk.c
├── zen.html
├── icon.png
├── list_files.php
├── launch.php
├── Makefile
└── README.md
```

## 🧾 Licencia

MIT

## ✍️ Créditos

Creado por **Garou**, arquitecto de sistemas simbólicos.  
Desarrollado en colaboración con **Microsoft Copilot**, compañero de creación, documentación.

---

**EN | English**

Zen Kiosk is a ceremonial, minimalist browser. Designed for microdistributions, it opens local files, modern WebApps, and executes `.jar` applications as external presences.

## ✨ Features

- Load local files (`.html`, `.mp4`, etc.)
- Open modern WebApps with support for:
  - JavaScript and WebAssembly
  - WebRTC and MediaStream
  - IndexedDB and local storage
  - PWA (Progressive Web Apps)
- Execute `.jar` files via `java -jar`
- Incognito mode by default (no history or cookies)
- Optional visual interface with icon grid (`zen.html`)
- Configurable search engine selector (Google, DuckDuckGo, Brave, Ecosia)

## 🛠️ Build

```bash
sudo apt install libwebkit2gtk-4.0-dev
make
```

## 🚀 Run

```bash
./zen_kiosk google.com
./zen_kiosk ./file.html
./zen_kiosk ./app.jar
```

## 📁 Recommended structure

```
zen_kiosk/
├── src/zen_kiosk.c
├── zen.html
├── icon.png
├── list_files.php
├── launch.php
├── Makefile
└── README.md
```

## 🧾 License

MIT

## ✍️ Credits

Created by **Garou**, architect of symbolic systems.  
Developed in collaboration with **Microsoft Copilot**, companion in creation, documentation.
```