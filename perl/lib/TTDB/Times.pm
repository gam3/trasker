use strict;
package TTDB::Times;

use TTDB::Time;

use TTDB::DBI qw (get_dbh);
use Params::Validate qw (validate);

sub new
{
    my $class = shift;

    bless {}, $class;
}

sub today
{
    my $self = shift;
    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select timeslice.id,
       user.name user_name,
       project.name,
       start_time,
       timediff(ifnull(end_time, now()), start_time) elapsed,
       ifnull(end_time, now()) end_time,
       'eof'
  from timeslice, project, user
 where user.id = timeslice.user_id
   and project.id = project_id
   and date(now()) >= date(start_time)
   and (date(now()) <= date(end_time) or end_time is null)
 order by start_time desc
 limit 100
SQL

    $sth->execute();

    my @data;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'TTDB::Time');
    }

    @data;
}

sub day
{
    my $self = shift;
    my $dbh = get_dbh();

    my $date = '2006-10-06';

    my $sth = $dbh->prepare(<<SQL);
select timeslice.id,
       user.name user_name,
       project.name,
       start_time,
       timediff(ifnull(end_time, now()), start_time) elapsed,
       ifnull(end_time, now()) end_time,
       'eof'
  from timeslice, project, user
 where user.id = timeslice.user_id
   and project.id = project_id
   and date(now()) >= date(start_time)
   and (date(now()) <= date(end_time) or end_time is null)
 order by start_time desc
 limit 100
SQL

    $sth->execute($date, $date);

    my @data;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'TTDB::Time');
    }

    @data;
}

sub entries
{
    my $self = shift;
    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select timeslice.id,
       user.name user_name,
       project.name,
       start_time,
       timediff(ifnull(end_time, now()), start_time) elapsed,
       ifnull(end_time, now()) end_time,
       'eof'
  from timeslice, user, project
 where user.id = user_id
   and project.id = project_id
 order by start_time desc
 limit 200
SQL

    $sth->execute();

    my @data;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'TTDB::Time');
    }

    @data;
}

sub all_day
{
    my $self = shift;
    my %p = validate(@_, {
        date => { isa => 'Date::Calc' },
    });

    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select project.id project_id,
       timeslice.user_id,
       project.name project_name,
       sec_to_time(
        sum(
	 time_to_sec(
	  timediff(
	   if(date(end_time) = '2006-10-31', end_time, '2006-10-31 24:00:00'),
	   if(date(start_time) = '2006-10-31', start_time, '2006-10-31 00:00:00')
	  )
	 )
        )
       )
       'eof'
  from timeslice, project
 where project.id = project_id
   and date(start_time) <= '2006-10-31' and (date(end_time) >= '2006-10-31' or end_time is null)
   and timeslice.user_id = 1
 group by project_id, timeslice.user_id
SQL

    $sth->execute();

    my @data;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'TTDB::Time');
    }

    @data;
}

1;
__END__

=head1 NAME

TTDB::Times - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

=head1 DESCRIPTION

=head2 Constructors

=over

=item new()

=back

=head2 Methods

=over

=item all_day

=item day

=item entries

=item today

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

