
# LIBSVN_FOUND
# LIBSVN_INCLUDE_DIR
# LIBSVN_LIBRARIES

find_path(LIBSVN_INCLUDE_DIR NAMES subvesion-1/svn_types.h PATH_SUFFIXES/subversio-1)

find_library(LIBSVN_CLIENT   NAMES libsvn_client-1)
find_library(LIBSVN_FS_UTIL  NAMES libsvn_fs_util-1)
find_library(LIBSVN_REPOS    NAMES libsvn_repos-1)
find_library(LIBSVN_DELTA    NAMES libsvn_delta-1)
find_library(LIBSVN_RA       NAMES libsvn_ra-1)
find_library(LIBSVN_SUBR     NAMES libsvn_subr-1)
find_library(LIBSVN_DIFF     NAMES libsvn_diff-1)
find_library(LIBSVN_RA_LOCAL NAMES libsvn_ra_local-1)
find_library(LIBSVN_WC       NAMES libsvn_wc-1)
find_library(LIBSVN_FS       NAMES libsvn_fs-1)
find_library(LIBSVN_RA_SERF  NAMES libsvn_ra_serf-1)
find_library(LIBSVN_FS_FS    NAMES libsvn_fs_fs-1)
find_library(LIBSVN_RA_SVN   NAMES libsvn_ra_svn-1)

set(LIBSVN_LIBRARIES ${LIBSVN_CLIENT} ${LIBSVN_FS_UTIL} ${LIBSVN_REPOS} ${LIBSVN_DELTA} ${LIBSVN_RA} ${LIBSVN_SUBR} ${LIBSVN_DIFF}
 ${LIBSVN_RA_LOCAL} ${LIBSVN_WC} ${LIBSVN_FS} ${LIBSVN_RA_SERF} ${LIBSVN_FS_FS} ${LIBSVN_RA_SVN})

