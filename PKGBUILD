# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=v1.0.0.r0.g9245424
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
groups=()
depends=()
makedepends=('gcc' 'git' 'libcurlpp' 'crypto++')
checkdepends=()
optdepends=()
provides=('riceman')
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=('riceman::git://github.com/ricelinux/riceman.git')
noextract=()
md5sums=('SKIP')
validpgpkeys=()

pkgver() {
	cd "$pkgname"
  	git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
}

build() {
	cd "$pkgname"
	make
}

package() {
	cd "$pkgname"
	install -Dm755 ./riceman "$pkgdir/usr/bin/riceman"
}
