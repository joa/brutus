# V8 inspired Makefile

CC := clang++
CXX := clang++

LINK ?= clang++
OUTDIR ?= out
TESTJOBS ?=
GYPFLAGS ?=
TESTFLAGS ?=

# Architectures and modes (e.g.: x64.release)
ARCHES = x64
DEFAULT_ARCHES = x64
MODES = release debug

# Dependencies to GYP files to re-trigger build
GYPFILES = build/all.gyp src/brutus.gyp

# ARCHES x MODES
BUILDS = $(foreach mode,$(MODES),$(addsuffix .$(mode),$(ARCHES)))

# Generates corresponding test targets, e.g. "x64.release.check".
CHECKS = $(addsuffix .check,$(BUILDS))

.SECONDEXPANSION:
$(MODES): $(addsuffix .$$@,$(DEFAULT_ARCHES))

$(ARCHES): $(addprefix $$@.,$(MODES))

# Defines how to build a particular target (e.g. x64.release).
$(BUILDS): $(OUTDIR)/Makefile.$$(basename $$@)
	@$(MAKE) -C "$(OUTDIR)" -f Makefile.$(basename $@) \
	         CXX="$(CXX)" LINK="$(LINK)" \
	         BUILDTYPE=$(shell echo $(subst .,,$(suffix $@)) | \
	                     python -c "print raw_input().capitalize()") \
	         builddir="$(shell pwd)/$(OUTDIR)/$@"

native: $(OUTDIR)/Makefile.native
	@$(MAKE) -C "$(OUTDIR)" -f Makefile.native \
	         CXX="$(CXX)" LINK="$(LINK)" BUILDTYPE=Release \
	         builddir="$(shell pwd)/$(OUTDIR)/$@"

# Clean

$(addsuffix .clean, $(ARCHES) $(ANDROID_ARCHES)):
	rm -f $(OUTDIR)/Makefile.$(basename $@)
	rm -rf $(OUTDIR)/$(basename $@).release
	rm -rf $(OUTDIR)/$(basename $@).debug
	find $(OUTDIR) -regex '.*\(host\|target\).$(basename $@)\.mk' -delete

native.clean:
	rm -f $(OUTDIR)/Makefile.native
	rm -rf $(OUTDIR)/native
	find $(OUTDIR) -regex '.*\(host\|target\).native\.mk' -delete

clean: $(addsuffix .clean, $(ARCHES) $(ANDROID_ARCHES)) native.clean

OUT_MAKEFILES = $(addprefix $(OUTDIR)/Makefile.,$(ARCHES))
$(OUT_MAKEFILES): $(GYPFILES) $(ENVFILE)
	GYP_GENERATORS=make \
	build/gyp/gyp --generator-output="$(OUTDIR)" build/all.gyp \
	              -Ibuild/standalone.gypi --depth=. \
	              -Dbrutus_target_arch=$(subst .,,$(suffix $@)) \
	              -S.$(subst .,,$(suffix $@)) $(GYPFLAGS)

$(OUTDIR)/Makefile.native: $(GYPFILES) $(ENVFILE)
	GYP_GENERATORS=make \
	build/gyp/gyp --generator-output="$(OUTDIR)" build/all.gyp \
	              -Ibuild/standalone.gypi --depth=. -S.native $(GYPFLAGS)

# Replaces the old with the new environment file if they're different, which
# will trigger GYP to regenerate Makefiles.
$(ENVFILE): $(ENVFILE).new
	@if test -r $(ENVFILE) && cmp $(ENVFILE).new $(ENVFILE) >/dev/null; \
	    then rm $(ENVFILE).new; \
	    else mv $(ENVFILE).new $(ENVFILE); fi

# Stores current GYPFLAGS in a file.
$(ENVFILE).new:
	@mkdir -p $(OUTDIR); echo "GYPFLAGS=$(GYPFLAGS)" > $(ENVFILE).new; \
	    echo "CXX=$(CXX)" >> $(ENVFILE).new
                                                                                              
dependencies:
	svn checkout --force http://gyp.googlecode.com/svn/trunk build/gyp \
		--revision 1451
