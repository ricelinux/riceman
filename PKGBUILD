# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=v0.0.1.5
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
depends=('curl' 'crypto++' 'fmt' 'libgit2')
makedepends=('clang' 'git' 'cmake' 'make' 'cpptoml')
provides=('riceman')
source=('git+https://github.com/ricelinux/riceman.git#commit=538f3e2417715bd6ab3281eb682728fbfc882b31')
noextract=()
md5sums=('SKIP')
install="$pkgname.install"

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
	make release
}

package() {
	cd "$pkgname"
	install -Dm755 ./riceman "$pkgdir/usr/bin/riceman"
}