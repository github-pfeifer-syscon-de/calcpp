
SUBDIRS = res srcLib src po

dist_doc_DATA = \
	README \
	README_de \
	NEWS \
	ChangeLog \
	COPYING \
	AUTHORS \
	INSTALL

ACLOCAL_AMFLAGS = --install -I m4

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
