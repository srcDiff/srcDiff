
# LIBSVN_FOUND
# LIBSVN_INCLUDE_DIR
# LIBSVN_LIBRARIES

find_path(LIBSN_INCLUDE_DIR NAMES subvesion-1/svn_types.h PATH_SUFFIXES/subversio-1)

set(LIBSVN_INCLUDE /usr/local/include/subversion-1;/usr/include/apr-1)
set(LIBSVN_LIBRARIES /usr/local/lib/libsvn_client-1.dylib;/usr/local/lib/libsvn_fs_util-1.dylib;/usr/local/lib/libsvn_repos-1.dylib;/usr/local/lib/libsvn_delta-1.dylib;/usr/local/lib/libsvn_ra-1.dylib;/usr/local/lib/libsvn_subr-1.dylib;/usr/local/lib/libsvn_diff-1.dylib;/usr/local/lib/libsvn_ra_local-1.dylib;/usr/local/lib/libsvn_wc-1.dylib;/usr/local/lib/libsvn_fs-1.dylib;/usr/local/lib/libsvn_ra_serf-1.dylib;/usr/local/lib/libsvn_fs_fs-1.dylib;/usr/local/lib/libsvn_ra_svn-1.dylib;libapr-1.dylib;libaprutil-1.dylib)
add_definitions(-DSVN)