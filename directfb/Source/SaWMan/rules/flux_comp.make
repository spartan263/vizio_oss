$(srcdir)/%.cpp $(srcdir)/%.h: $(srcdir)/%.flux
	fluxcomp $<
