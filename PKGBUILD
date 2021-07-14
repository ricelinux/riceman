# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=v1.0.0.r62.g3dbc862
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
groups=()
depends=()
makedepends=('gcc' 'git' 'curl' 'crypto++' 'cmake' 'make')
checkdepends=()
optdepends=()
provides=('riceman')
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=('git+https://github.com/ricelinux/riceman.git'
		'git+https://github.com/jpbarrette/curlpp.git')
noextract=()
md5sums=('SKIP')
validpgpkeys=()

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
	install -Dm644 deps/curlpp/libcurlpp.so.1 /usr/lib/libcurlpp.so.1
}
