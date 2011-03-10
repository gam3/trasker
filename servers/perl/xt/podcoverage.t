use Test::More;
eval "use Test::Pod::Coverage";
plan skip_all => "Test::Pod::Coverage required for testing pod coverage" if $@;

plan tests => 2;
pod_coverage_ok( "Trasker::TTDB::User" );
pod_coverage_ok( "Trasker::TTDB::Project" );
