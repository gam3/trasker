use strict;
use warnings;
use Module::Build;

# Tasker version 0.0.1

my $builder = Module::Build->new(
    module_name   => 'Tasker::TTCP',
    dist_version => '0.01',
    license       => 'perl',
    requires => {
        'perl'          => '5.6.0',
    },
    script_files => [ 'scripts/get_gaim' ],
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


