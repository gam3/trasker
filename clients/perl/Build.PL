use strict;
use warnings;
use Module::Build;

# Tasker version 0.0.1

my $builder = Module::Build->new(
    module_name   => 'Trasker::TTCP',
    dist_version => '0.01',
    dist_author => 'G. Allen Morris III <gam3-cpan@gam3.net>',
    dist_abstract => "Trasker perl clients and trasker perl library",
    license       => 'perl',
    requires => {
        'perl'          => '5.6.0',
    },
    script_files => [ 'scripts/' ],
    build_requires => {
	'Getopt::Long' => 0,
	'Pod::Usage' => 0,
        'Params::Validate' => 0,
    },
    recommends => {
        'Test::Pod' => 0,
        'Test::Pod::Coverage' => 0,
    },
    create_readme => 1,
    sign           => 1,
    create_makefile_pl => 'traditional',
    add_to_cleanup => [ 'TurboSchedule-*' ],
);
$builder->create_build_script;


