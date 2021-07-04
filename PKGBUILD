# Maintainer: Zaedus <nintendozaedus@gmail.com>
pkgname='riceman'
pkgver=1.0.0
pkgrel=1
pkgdesc="The official rice manager for Rice Linux"
arch=("x86_64")
url="https://github.com/ricelinux/riceman"
license=('GPL')
groups=()
depends=()
makedepends=('gcc' 'git')
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
  	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd "$pkgname"
	make
}

package() {
	cd "$pkgname"
	install -Dm755 ./riceman "$pkgdir/usr/bin/riceman"
}
