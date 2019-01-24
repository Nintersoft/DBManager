/*-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 15 of November of 2018
#
------------------------------------------------- */

#ifndef DBMANAGER_GLOBAL_H
#define DBMANAGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DBMANAGER_LIBRARY)
#  define DBMANAGERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DBMANAGERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DBMANAGER_GLOBAL_H
