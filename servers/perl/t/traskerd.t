use Test::More qw(no_plan);

$ENV{TEST_TRASKERD} = 1;
require 'script/traskerd';

is($0, 'traskerd', "name");

use Data::Dumper;

#diag Dumper \%INC;
