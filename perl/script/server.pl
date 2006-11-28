#!/usr/bin/perl

use strict;
use POSIX ;
use Socket;
use IO::Socket;
use IO::Socket::SSL qw(debug3);
use IO::Select;
use Fcntl qw(F_GETFL F_SETFL O_NONBLOCK O_NDELAY);

use Net::TCPwrappers qw(RQ_DAEMON RQ_FILE request_init fromhost hosts_access);

use Getopt::Long;            # support options+arguments
use Pod::Usage;              # avoid redundant &Usage()

my $done=0;
my $select;
my $server;
my $sslserver;
my $connections=0;

our $progname = 'tskrsrvr';

sub print_log {
    print strftime("%b %e %T", localtime)." ";
    print @_;
    print "\n";
}

use TTDB::Entry;
use TTDB::User;
use TTDB::Project;

use TTDB::DBI qw (dbi_setup);

sub nonblock {
    my $socket = shift;
    my $flags;
print join(' ', caller) unless $socket;
    return unless $socket;
    $flags = fcntl($socket, F_GETFL, 0)
        or print_log "Can't get flags for socket: $!\n";
    fcntl($socket, F_SETFL, $flags | O_NONBLOCK)
        or print_log "Can't make socket nonblocking: $!\n";
}

################################################################################

our $hash = {};
our $user_hash = {};
our $datahash = {};

our $time_count;

our $opt_debug;
our $opt_help;
our $opt_man;
our $opt_versions;

sub get_user
{
    my $userid = shift or die 'Need a user';
    my $client = shift or die 'Need a client';
    my $user;

    if ($hash->{_user}{$userid}) {
	$user = $hash->{_user}{$userid};
    }
    unless ($user) {
	$user = TTDB::User->new(user => $userid);
	$hash->{_user}{$userid} = $user;
    }
    $user;
}

our $cmd_hash = {
    authorize => \&authorize,		# get authorization for client
    addtask => \&addtask,		# add a task for a user
    change => \&change,			# change current project
    clients => \&clients,		# list clients
    desktop => \&desktop,
    gettime => \&gettime,
    help => \&help,
    note => \&note,
    nop => \&nop,
    pop => \&xpop,
    project => \&project,
    push => \&xpush,
    quit => \&quit,
    tracker => \&tracker,
    track => \&track,
    user => \&user,
    update => \&update,
    xtracker => \&xtracker,
};

sub help()
{
    my $client = shift;
    print($client <<EOP);
user\t{username}/{password}
change\t{project id}
track
EOP
    '';
}

sub nop
{
    my $client = shift;
    my $data = shift;

    print($client join(' ', @_), "\n");
    print(join(' ', @_), "\n");
    '';
}

sub note
{
    my $client = shift;
    my $data = shift;
    my $user = shift;
    my $project_id = shift;

    my ($a, $b, $c, $note) = split('\t', $data);

    my $userobj = get_user($user, $client);
    my $noteobj;

    if ($project_id) {
	$noteobj = $userobj->project(project_id => $project_id);
    } else {
	$noteobj = $userobj;
    }

    if ($noteobj->add_note(note => $note)) {
	printf($client "accept_note\tOK\n");
    } else {
	printf($client "error\tCould not add note.\n");
    }
}

sub project
{
    my $client = shift;
    my $data = shift;
    my $comand = shift;
    my $user = shift;
    my $parent = shift;

    print "$comand $user $parent\n";
}

sub addtask
{
    my $client = shift;
    my $data = shift;
    my $user = shift;
    my $project_id = shift;

    my ($a, $userx, $parent_id, $name, $description) = split('\t', $data);

    warn "$name: $description";
    warn "$data";

    my $userobj = get_user($user, $client);
    my $userproj = $userobj->project(project_id => $parent_id);

    if (my $up = $userproj->add_task(
	name => $name,
        description => $description)
       ) {
	printf($client "accept_task\tOK\n");
    } else {
	printf($client "error\tCould not add project.\n");
    }
    return undef;
}

sub gettime
{
    my $client = shift;
    my $data = shift;
    my $user_id = shift;
    my $id = shift;

    die "Bad id: $id" unless ($id =~ m/^\d+$/);

    my $user = get_user($user_id, $client);

    my $time;
    my $atime;

    my $project = $user->project(project_id => $id);

    eval {
	$time = $project->time();
	$atime = $project->alltime();
    }; if ($@) {
        warn "ID: $user_id $id ", $@;
	return '';
    }

    printf($client "update_time\t%s\t%d\t%s\t%s\n", $user_id, $id, $time, $atime);
    undef;
}

sub quit
{
    my $client = shift;
    print($client "Bye, bye.\n");
    'QUIT';
}

sub track
{
    my $client = shift;
    my $data = shift;
    my $user = shift;

    if ($user_hash->{$user}->{'track'}->{$client}) {
print $client "Already tracking\n";
    } else {
print $client "tracking\n";
$user_hash->{$user}->{'track'}->{$client} = $client;
    }
    '';
}

sub desktop
{
    my $client = shift;
    my $data = shift; # data
    my $userid = shift;
    my $host = shift;
    my $start_utime = shift;
    my $desktop_id = shift;
    my $desktop = shift;

    my $user = get_user($userid, $client);
die;
print "Desktop $desktop\n";

    if (my $pid = $user->auto_set_project(host => $host, desktop => $desktop)) {
	update_user($user);
    }
    return undef;
}

sub tracker
{
    my $client = shift;
    my $data = shift; # data
    my $userid = shift;
    my ($host1, $idle, $desktop, $window, $host, $class, $title, $dummy, $name) = @_;

    my $user = get_user($userid, $client);

print "Tracker $desktop [$idle] $host, $class, $title ($dummy) $name\n";

    if ($idle) {
	if (my $pid = $user->auto_revert_project(host => $host,
					      desktop => $desktop,
					      class => $class,
					      title => $title,
					      name => $name,
					     )
	   ) {
	    update_user($user);
	}
    } else {
	if (my $pid = $user->auto_set_project(host => $host,
					      desktop => $desktop,
					      class => $class,
					      title => $title,
					      name => $name,
					     )
	   ) {
	    update_user($user);
	}
    }

    return undef;
}

sub xtracker
{
    shift;
    shift; # data
    my $user = shift;

    for my $clientk (keys %{$user_hash->{$user}{'track'}}) {
        my $client = $user_hash->{$user}{'track'}{$clientk};
        print($client "Tracker ", join(' ', @_), "\n");
    }
    '';
}

sub user
{
    my $client = shift;
    my $data = shift;
    my $userid = shift;

    my $user = get_user($userid, $client);

    if (exists $hash->{_clients}{$client}{_list}{$user->name}) {
	print($client "user $user\n");
    } else {
	$hash->{_clients}{$client}{_list}{$user->name} = $user;

	for my $entry ($user->projects) {
	    my $time = $entry->time;
	    my $atime = $entry->alltime;

	    printf($client "entry\t%s\t%s\t%d\t%d\t%s\t%s\t%s\n",
		   $userid,
		   $entry->name,
		   $entry->project->id,
		   $entry->project->parent_id,
		   $time, $atime,
		   $entry->active
	    );
	}
    }
    if (my $current_proj = $user->current_project()) {
	printf($client "current\t%s\t%d\n", $userid, $current_proj->project->id);
    }
    '';
}

sub clients
{
    my $client = shift;
    my $data = shift;
    my $userid = shift;

use Data::Dumper;
warn 'clients ', Dumper $user_hash, $hash;
    
    my $user = get_user($userid, $client);

    warn 'clients ', Dumper $user;

    for my $uclient (keys %{$hash}) {
	print($client "client $uclient\n");
    }
}

sub xpush
{
    my $client = shift;
    my $data = shift;
    my $user = shift;
    my $value = shift;

    die;

    printf("Push $user $value\n");
    get_user($user, $client)->set_current_project(project_id => $value);
    for my $uclient (keys %{$hash}) {
	print("$uclient current $user $value\n");
	next unless $hash->{$uclient}{'_client'};
	my $cli = $hash->{$uclient}->{'_client'};
	print($cli "current\t$user\t$value\n");
    }
}

sub xpop
{
    my $client = shift;
    my $data = shift;
    my $user = shift;

    get_user($user, $client)->revert();
}

sub get_clients_for
{
    my $name = shift;
    my $user = shift;

    grep({ $_->{$name}{$user->name} } map({ $hash->{_clients}{$_} } %{$hash->{_clients}}));
}

sub update
{
    my $client = shift;

warn Dumper $client;
    for my $clientd (map({ $hash->{_clients}{$_} } keys %{$hash->{_clients}})) {
use Data::Dumper;
warn Dumper $clientd;
        my $cli = $clientd->{client};
	printf($cli "update\n");
    }
}

sub update_user
{
    my $user = shift;
    die 'Bad user: ', join(' ', caller) unless $user->isa('TTDB::User');

    my $user_project = $user->current_project();
    my $user_name = $user->name;

    my $id = $user_project->project->id;

    my @clients = get_clients_for('_list', $user);

    $hash->{'_current'} = $id;

    for my $chash (@clients) {
	my $cli = $chash->{client};

	print($cli "current\t$user_name\t$id\n");
    }
}

sub change
{
    my $client = shift;
    my $data = shift;
    my $user = shift;
    my $value = shift;
    my $peername = $client->peername;
    my ($port, $iaddr) = sockaddr_in($peername);
    my $host = gethostbyaddr($iaddr, AF_INET);

    my $project = TTDB::Project->new(id => $value);
    my $user = get_user($user, $client);
    $user->set_current_project(project => $project, host => 'unknown');

    update_user($user);
}

sub main
{
    $| = 1;
    $done=0;

    dbi_setup(
        database => 'tasker',
	user => 'gam3',
	password => 'awnmwad',
    );
    for my $sig (keys %SIG) {
        next if $sig eq 'TSTP';
        $SIG{$sig} = sub { print_log('Signal '.$_[0].' caught!'); };
    }

    $SIG{__WARN__} = sub { print_log('WARNING: ' . $_[0]); };
    $SIG{__DIE__} = sub { print_log('DIE: ' . $_[0]); };
    $SIG{PIPE} = 'IGNORE';
    $SIG{INT} = $SIG{TERM} = $SIG{QUIT} = $SIG{ABRT} = sub { $done = 1; };
        
    $server = IO::Socket::INET->new(LocalPort => 8000,
                    Listen => 10,
                    Proto => "TCP",
                    ReuseAddr => 1,
                    )
        or die "Can't create a server socket: $@";
    $sslserver = IO::Socket::SSL->new(LocalPort => 8001,
                    Listen => 10,
                    Proto => "TCP",
                    ReuseAddr => 1,
		    SSL_key_file => '/home/gam3/demoCA/private/cakey.pem',
		    SSL_cert_file => '/home/gam3/demoCA/cacert.pem',
                    )
        or die "Can't create a server socket: $@";
    setsockopt($server,SOL_SOCKET,SO_LINGER,pack("l*",0,0)) || return undef;
    nonblock($server);
    $select = IO::Select->new();
    $select->add($server);
    $select->add($sslserver);
        
    print_log "Started";

    daemonize() unless $opt_debug;

    my $line_count;
    my $x = 0;
    while (!$done) {
        my $client;
        my $rv;
        my $data;
	my $ssl = 0;

        foreach $client ($select->can_read(1)) {
            if ($client == $server || $client == $sslserver) {
                if ( $connections < 10 ) {
		    if ($client == $sslserver) {
			$client = $sslserver->accept();
			$ssl = 1;
print "accept $client\n";
		    } else {
			$client = $server->accept();
		    }
                    next unless $client;
		    my $req = request_init(RQ_DAEMON, $progname, RQ_FILE, $client->fileno());
		    fromhost($req);

		    if (!hosts_access($req)) {
			print $client "The maximum number of simultaneous connections reached.\n";
			$client->shutdown(2);
			close($client);
		    } else {
			$connections++;
			$select->add($client);
			nonblock($client);
			print_log(sprintf("Connected: %s:%s", $client->peerhost || 'N/A', $client->peerport || -1));
			print($client "TTCP\t0.01\n");
			$hash->{_clients}{$client} = { ssl => $ssl, client => $client };
		    }
                } else {
                    print_log("The maximum number of simultaneous connections reached");
                    $client = $server->accept();
                    print $client "The maximum number of simultaneous connections reached.\n";
                    $client->shutdown(2);
                    close($client);
                }
            } elsif ($client == $sslserver) {
                if ( $connections < 10 ) {
                    $client = $sslserver->accept();
                    next unless $client;
print "accept $client\n";
		    my $req = request_init(RQ_DAEMON, $progname, RQ_FILE, $client->fileno());
		    fromhost($req);
		    if (!hosts_access($req)) {
			print $client "The maximum number of simultaneous connections reached.\n";
			$client->shutdown(2);
			close($client);
		    } else {
			$connections++;
			$select->add($client);
			nonblock($client);
			print_log(sprintf("Connected (ssl): %s:%s", $client->peerhost || 'N/A', $client->peerport || -1));
			print($client "TTCP 0.01\n");
			$hash->{_clients}{$client} = { ssl => 0, client => $client };
		    }
                } else {
                    print_log("The maximum number of simultaneous connections reached");
                    $client = $server->accept();
                    print $client "The maximum number of simultaneous connections reached.\n";
                    $client->shutdown(2);
                    close($client);
                }
            } else {
                $data = '';
                $rv = $client->read($data, POSIX::BUFSIZ, 0);
                
                unless (defined($rv) && length $data) {
QUIT:
                    print_log(sprintf("Disconnected: %s:%s", $client->peerhost || 'N/A', $client->peerport || -1));
                    $select->remove($client);
                    $client->shutdown(2);
                    close $client;

		    delete $hash->{_clients}{$client};

                    $connections--;
                    next;
                }

                $data =~ s/\r//go;

		$datahash->{$client} .= "$data";
		while ($datahash->{$client} =~ s/([^\n]*)\n//) {
		    my $data = $1;
                    my ($cmd, @args);
                   
		    if ($data =~ /\t/) {
			($cmd, @args) = split('\t', $data);
		    } else {
			($cmd, @args) = split(' ', $data);
		    }

		    if (my $function = $cmd_hash->{$cmd}) {
			my $action;
		        eval {
			    $action = $function->($client, $data, @args);
			};
			if ($@) {
			    print "Error: ", $@;
			}
			if ($action eq 'QUIT') {
			    goto QUIT;
			}
		    } else {
			print("Warning no command $cmd ", join(' ', @args), "\n");
		    }
		}
	    }
        }
	# we are here one a second

	my (@time) = localtime;
	if ($time[1] == 0) {
	    my @clients;
#	    = grep({ $_->{_list}{$user->name} } map({ $hash->{_clients}{$_} } %{$hash->{_clients}}));
	    for my $client (@clients) {

            }
	}
    }

    print_log "Exit";

    $server->shutdown(2);
    close($server);
}

sub daemonize
{
    chdir '/'               or die "Can't chdir to /: $!";
    open STDIN, '/dev/null' or die "Can't read /dev/null: $!";
    open STDOUT, '>/dev/null'
                            or die "Can't write to /dev/null: $!";
    defined(my $pid = fork) or die "Can't fork: $!";
    exit if $pid;
    setsid                  or die "Can't start a new session: $!";
    open STDERR, '>&STDOUT' or die "Can't dup stdout: $!";
}

GetOptions(
    'debug=i'   => \$opt_debug,
    'help!'     => \$opt_help,
    'man!'      => \$opt_man,
    'versions!' => \$opt_versions,
) or pod2usage(-verbose => 1) && exit;

#pod2usage(-verbose => 1) && exit if ($opt_debug !~ /^[01]$/);
pod2usage(-verbose => 1) && exit if defined $opt_help;
pod2usage(-verbose => 2) && exit if defined $opt_man;

main;

__END__
=head1 NAME

 tskserver.pl

=head1 SYNOPSIS

 tskserver.pl

=head1 DESCRIPTION

 asdf

 eg:
   tskserver.pl
   tskserver --server=tskserver

=head1 OPTIONS

 --versions   print Modules, Perl, OS, Program info
 --debug 0    don't print debugging information (default)
 --debug 1    print debugging information

 --help      print Options and Arguments instead
 --man       print complete man page

=head1 API

The first step is that the server sends a version number;

The client should then authorize the user.


=head1 AUTHOR

G. Allen Morris III <gam3@gam3.net>

=head1 CREDITS

Gnotime convinced me to work on this project.

=head1 TESTED

 Pod::Usage            1.14
 Getopt::Long          2.2602
 Perl    5.00503
 Debian  2.2r5

=head1 BUGS

None that I know of.

=head1 TODO

  This is currently a very long list. 

=head1 UPDATES

 2006-04-07   12:05 EST
   Initial code

=cut

