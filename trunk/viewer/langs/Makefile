PERL            =   /usr/bin/perl
LANG            =   en de cs it fr ja fo da zh_CN pl ru es tr th ca no
RCPLINT         =   ../scripts/rcplint.pl
LN_S            =   ln -s

test:
	rm -f *.lint
	for rcp in *.rcp; do $(PERL) $(RCPLINT) $$rcp; done

clean:
	rm -f core *~

distclean: clean
	rm -f *.lint Makefile
