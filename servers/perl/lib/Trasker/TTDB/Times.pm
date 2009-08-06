use strict;
## @file
# The Time object
#

## @class
# Get time information
package Trasker::TTDB::Times;

use Trasker::TTDB::Time;

use Trasker::TTDB::DBI qw (get_dbh);
use Params::Validate qw (validate);

=head1 NAME

Trasker::TTDB::Times - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

=head1 DESCRIPTION

=head2 Constructors

=over

=item new()

=back

sub new
{
    my $class = shift;

    bless {}, $class;
}

sub get
{
    my $self = shift;
    my $dbh = get_dbh();
    my %p = validate(@_, {
        date => {
	    date => {
		isa => 'Date::Calc',
	    },
	},
    });

    my @args = ();

    my $date_clause = '';
    
    if ($p{date}) {
	$date_clause = <<EOP;
and start_time < ? and end_time >= ?
EOP
        push(@args, ($p{date} + 1)->mysql);
        push(@args, ($p{date} + 0)->mysql);
    }

    my $sth = $dbh->prepare(<<SQL);
select timeslice.id,
       users.name as user_name,
       project.id as project_id,
       project.name as project_name,
       start_time,
       end_time,
       'eof'
  from timeslice, project, users
 where users.id = timeslice.user_id
   and project.id = project_id
   $date_clause
 order by start_time
 limit 100
SQL

    $sth->execute(@args);

    my @data;

    require Trasker::TTDB::TimeSlice;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'Trasker::TTDB::TimeSlice');
    }

    @data;
}

=head2 Methods

=over

=item all_day

=item day

=item entries

=item today

1;
__END__

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

