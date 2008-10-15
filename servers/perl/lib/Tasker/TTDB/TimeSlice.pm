use strict;
## @file
## The Timeslice object
##
#

## @class
# A little slice of time
#
# A timeslice is a block of time attributed to a user and a project.
#
package Tasker::TTDB::TimeSlice;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Tasker::TTDB::DBI qw (get_dbh);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        start_time => {
            isa => 'Date::Calc',
        },
        end_time => {
            isa => 'Date::Calc',
        },
        user => {
            isa => 'Tasker::TTDB::User',
            optional => 1,
        },
        project => {
            isa => 'Tasker::TTDB::Project',
            optional => 1,
        },
        temporary => 0,
        host => 0,
        elapsed => 0,
        id => 0,
        user_id => 0,
        auto_id => 0,
        revert_to => 0,
        project_id => 0,
    });
    my $self = bless({ %p }, $class);

    return $self;
}

sub get
{
    my $class = shift;
    my $dbh = get_dbh();

    my %p = validate(@_, {
        start_time => {
            isa => 'Date::Calc',
        },
        end_time => {
            isa => 'Date::Calc',
        },
        user => {
            isa => 'Tasker::TTDB::User',
        }
    });
    my $self = bless({ %p }, $class);
}

sub _get
{
    my $self = shift;
    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select *
  from timeslice
 where (end_time > ? or end_time is null)
   and start_time <= ?
   and user_id = ?
SQL

    my $start = $self->start_time->mysql;
    my $end = $self->end_time->mysql;
    my $user_id = $self->user->id;

    $sth->execute( $start, $end, $user_id );

    my $data = $sth->fetchall_arrayref({});

    $self->{data} = $data;

    return $self;
}

sub id
{
    my $self = shift;
    
    $self->{id};
}

sub start_time
{
    my $self = shift;
    require Date::Calc::MySQL;

    Date::Calc::MySQL->new($self->{data}{start_time});
}

sub end_time
{
    my $self = shift;

    $self->{data}{end_time} ? Date::Calc::MySQL->new($self->{data}{end_time}) : Date::Calc::MySQL->now();
}

sub duration
{
    my $self = shift;

    $self->end_time - $self->start_time;
}

sub user
{
    shift->{user};
}

sub project
{
    my $self = shift;

    $self->{project} ||= Tasker::TTDB::Project->get(id => $self->{data}{project_id});
}

sub elapsed
{
    my $self = shift;

    $self->end_time - $self->start_time;
}

sub count
{
    my $self = shift;

    @{$self->{data}};
}

sub create
{
    my $self = shift;
    my $dbh = get_dbh('write');

    $self->_get();

    return "Can't create" if $self->count();
    
    my $st = $dbh->prepare(<<SQL);
insert into timeslice
       (user_id, project_id, start_time, end_time, elapsed)
values (?, ?, ?, ?, ?)
SQL

    $st->execute(
        $self->user->id,
        $self->project->id,
        $self->start_time->mysql,
        $self->end_time->mysql,
        $self->elapsed->mysql,
    );
}

sub update
{
    my $self = shift;
    my $dbh = get_dbh('write');

die;

    $self->_get();

    my $user_id = $self->user->id;
    my $start = $self->start_time->mysql;
    my $end = $self->end_time->mysql;

    my $st = $dbh->prepare(<<SQL);
select id,
       start_time,
       end_time,
       user_id,
       project_id,
       'eof'
  from timeslice
 where (end_time > ? or end_time is null)
   and start_time < ?
   and user_id = ?
   for update
SQL

    $st->execute($start, $end, $user_id);

    my @ids = map({ { %$_, start_time => Date::Calc::MySQL->new($_->{start_time}), end_time => Date::Calc::MySQL->new($_->{end_time}), } } @{$st->fetchall_arrayref({})});

    my $ins = 0;
    for my $ts (@ids) {
        if ( $self->start_time eq $ts->{start_time} &&
            $self->end_time eq $ts->{end_time}) {
        } elsif ( $self->start_time ge $ts->{start_time} &&
#new time inside old time
            $self->end_time le $ts->{end_time}) {
            if ( $self->start_time gt $ts->{start_time} ) {
                my $sta = $dbh->prepare(<<SQL);
update timeslice set end_time = ?, elapsed = ? where id = ?
SQL
                my $el = ($self->end_time - $ts->{start_time});
                $sta->execute($self->start_time->mysql, ($self->start_time - $ts->{start_time})->mysql, $ts->{id});
            } else {
warn "start eq";
            }
            if ( $self->end_time lt $ts->{end_time} ) {
                my $sti = $dbh->prepare(<<SQL) or die;
insert into timeslice
       (user_id, project_id, start_time, end_time, elapsed)
values (?, ?, ?, ?, ?)
SQL
                $sti->execute(
                    $ts->{user_id},
                    $ts->{project_id},
                    $self->end_time->mysql,
                    $ts->{end_time}->mysql,
                    ($ts->{end_time} - $self->end_time)->mysql
                ) or die;
            } else {
warn "end eq";
            }
            my $sta = $dbh->prepare(<<SQL);
insert into timeslice
       (user_id, project_id, start_time, end_time, elapsed)
values (?, ?, ?, ?, ?)
SQL
            $sta->execute(
                $self->user->id,
                $self->project->id,
                $self->start_time->mysql,
                $self->end_time->mysql,
                $self->elapsed->mysql,
            );
        } elsif (
            $self->start_time gt $ts->{start_time} &&
            $self->start_time lt $ts->{end_time}
        ) {
# set the end time of the first block
            my $sta = $dbh->prepare(<<SQL);
update timeslice set end_time = ?, elapsed = ? where id = ?
SQL
            $sta->execute($self->start_time->mysql, ($self->start_time - $ts->{start_time})->mysql, $ts->{id});
            $ins++;
        } elsif (
            $self->end_time gt $ts->{start_time} &&
            $self->end_time lt $ts->{end_time}
        ) {
# set the end time of the first block
            my $sta = $dbh->prepare(<<SQL);
update timeslice set start_time = ?, elapsed = ? where id = ?
SQL
            $sta->execute($self->end_time->mysql, ($ts->{end_time} - $self->end_time)->mysql, $ts->{id});
            $ins++;
        } else {
            my $sta = $dbh->prepare(<<SQL);
delete from timeslice where id = ?
SQL
            $sta->execute($ts->{id});
            $ins++;
        }
    }
    if ($ins) {
#        $self->create;
        my $st = $dbh->prepare(<<SQL);
insert into timeslice
       (user_id, project_id, start_time, end_time, elapsed)
values (?, ?, ?, ?, ?)
SQL

        $st->execute(
            $self->user->id,
            $self->project->id,
            $self->start_time->mysql,
            $self->end_time->mysql,
            $self->elapsed->mysql,
        );
    }
    $dbh->commit;
}

sub notes
{
    my $self = shift;
    require Tasker::TTDB::Notes;
    
    Tasker::TTDB::Notes->new(
        start_time => $self->start_time,
        end_time => $self->end_time,
    );
}

1;
__END__

=head1 NAME

Tasker::TTDB::TimeSlice - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

  use Tasker::TTDB::User;

  $user = Tasker::TTDB::User->new(user => 'bob', fullname => 'Robert Smith'):

  $user->create();

  $user = Tasker::TTDB::User->get(user => 'bob'):
  $user = Tasker::TTDB::User->get(id => 1):

=head1 DESCRIPTION

=head2 Constructors

=over

=item new()

=back

=head2 Methods

=over

=item count

=item create

=item duration

=item elapsed

=item end_time

=item get

=item id

=item project

=item start_time

=item update

=item user

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
