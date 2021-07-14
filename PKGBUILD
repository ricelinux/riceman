# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=v1.0.0.r62.g3dbc862
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
makedepends=('gcc' 'git' 'curl' 'crypto++' 'cmake' 'make')
provides=('riceman')
source=('git+https://github.com/ricelinux/riceman.git')
noextract=()
md5sums=('SKIP')

pkgver() {
	cd "$pkgname"
  	git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
}

prepare() {
	cd "$pkgname"
	git submodule update --init --recursive

	cd deps/curlpp
	cmake .
	make
}

build() {
	cd "$pkgname"
	make
}

package() {
	cd "$pkgname"
	install -Dm755 ./riceman "$pkgdir/usr/bin/riceman"
	install -Dm644 ./deps/curlpp/libcurlpp.so.1 "$pkgdir/usr/lib/libcurlpp.so.1"
}
