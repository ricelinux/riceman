# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=0
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
depends=('curl' 'crypto++' 'fmt' 'libgit2')
makedepends=('clang' 'git' 'cmake' 'make' 'cpptoml')
provides=('riceman')
source=('git+https://github.com/ricelinux/riceman.git#commit=edb81944f8b03038d198fb18055e8d100977b5c9')
noextract=()
md5sums=('SKIP')

pkgver() {
	cd "$pkgname"
  	git describe --long | sed -r 's/-([0-9,a-g,A-G]{7}.*)//' | sed 's/-/./g'
}

prepare() {
	cd "$pkgname"
	git submodule update --init --recursive
}

build() {
	cd "$pkgname"
	make
}

package() {
	cd "$pkgname"
	install -Dm755 ./riceman "$pkgdir/usr/bin/riceman"
}

post_install() {
	if [ ! $(getent group | grep $pkgname) ]; then
		useradd -M -N -U -r $pkgname
	fi
}

post_remove() {
	userdel -f $pkgname
	groupdel $pkgname
}