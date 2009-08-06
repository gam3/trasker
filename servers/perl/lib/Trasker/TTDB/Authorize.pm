use strict;
use warnings;

package Trasker::TTDB::Authorize;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Trasker::TTDB::DBI qw (get_dbh);

=head1 NAME

Trasker::TTDB::Authorize - Perl interface to the tasker permition system

=head1 SYNOPSIS

  use Trasker::TTDB::Authorize;

  my $auth = Trasker::TTDB::Authorize->authorize(user => 'bob', password => 'xxxxx'):

  if ($auth->fail()) {
      exit();
  }

=head2 Constructor

=over

=item authorize(name => I<required>, password => I<required>)

=cut

sub authorize
{
    my $class = shift;
    my $dbh = get_dbh();
    my %p = validate(@_, {
        login => 1,
        password => 1,
    });
    my $ok = 0;
    if ($p{login} eq 'gam3' and $p{password} eq 'ab12cd34') {
        $ok = 1;
    }
    return bless { ok => $ok, @_ }, 'Trasker::TTDB::Authorize';
}

sub fail
{
    my $self = shift;
    !$self->{ok};
}

sub ok
{
    my $self = shift;
    !!$self->{ok};
}

1;

=back

=head1 AUTHOR

G. Allen Morris III <gam3@gam3.net>

=head1 BUGS

Nothing but.

=head1 TODO

This is currently a very long list.

=cut



