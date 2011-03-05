use inc::Module::Install;

# Tasker version 0.0.5
# /usr/local/share/perl/5.10.0

install_as_vendor;

# Define metadata
name 'Trasker';
version '0.0.5';
license 'GPL';

requires 'IO::Socket::SSL' => 0;
requires 'Config::Simple' => 0;

test_requires 'Getopt::Long' => 0;
test_requires 'Pod::Usage' => 0;
test_requires 'Params::Validate' => 0;

recommends 'Test::Pod' => 0;
recommends 'Test::Pod::Coverage' => 0;
recommends 'Test::Spelling' => 0;
recommends 'WWW::Mechanize' => 0;

build_requires 'Module::Install::AuthorTests';
build_requires 'Module::Install::AutoConf';

sign 1;

install_script 'traskerd';
install_script 'traskers';

die "Module::Install::AutoConf must be installed" unless (defined $Module::Install::AutoConf::VERSION);

makefile('pmakefile');

author_tests('xt') if (defined $Module::Install::AuthorTests::VERSION);

WriteAll(
    sign => $ENV{SIGN}
);

