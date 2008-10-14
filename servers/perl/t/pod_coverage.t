use Test::More;

eval "use Test::Pod::Coverage";
plan skip_all => "Test::Pod::Coverage required for testing pod coverage" if $@;

plan tests => 3;
pod_coverage_ok("TTDB::User");
pod_coverage_ok("TTDB::Project");
pod_coverage_ok("TTDB::Projects");

