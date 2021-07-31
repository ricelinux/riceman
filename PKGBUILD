# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=v1.0.0.r62.g3dbc862
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
depends=('curl' 'crypto++' 'fmt' 'libgit2')
makedepends=('clang' 'git' 'cmake' 'make')
provides=('riceman')
source=('git+https://github.com/ricelinux/riceman.git#commit=edb81944f8b03038d198fb18055e8d100977b5c9')
noextract=()
md5sums=('SKIP')

pkgver() {
	cd "$pkgname"
  	git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
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
