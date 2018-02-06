
TEMPLATE = subdirs
SUBDIRS = \
                        ../libamex-gpio-dev \
                        ../libamex-bus-dev \
                        ../libamex-module-adis16405-dev \
                        ../libamex-module-bmp280-dev \
                        ../libamex-module-neom8ngps-dev \
                        ../libamex-module-serialout-dev \
                        ../libamex-sucmder-dev \
                        ../libamex-entry-dev \
                        ../libSimpleCrypt \
                        ../libamex-license-dev \
                        ../libamex-mmcid-dev \
                        ../libNavout \
                        ../libqmodules \
                        ../Navigator-app

			
Navigator.depends = \
                        ../libamex-gpio-dev \
                        ../libamex-bus-dev \
                        ../libamex-module-adis16405-dev \
                        ../libamex-module-bmp280-dev \
                        ../libamex-module-neom8ngps-dev \
                        ../libamex-module-serialout-dev \
                        ../libamex-sucmder-dev \
                        ../libamex-entry-dev \
                        ../libSimpleCrypt \
                        ../libamex-license-dev \
                        ../libamex-mmcid-dev \
                        ../libNavout \
                        ../libqmodules



release: DESTDIR = ../build/release
debug:   DESTDIR = ../build/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
