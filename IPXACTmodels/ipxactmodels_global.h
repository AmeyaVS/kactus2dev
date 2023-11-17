#ifndef IPXACTMODELS_GLOBAL_H
#define IPXACTMODELS_GLOBAL_H

#include <QSharedPointer>
#include <QList>

#ifdef IPXACTMODELS_LIB
# define IPXACTMODELS_EXPORT Q_DECL_EXPORT
#else
# define IPXACTMODELS_EXPORT Q_DECL_IMPORT
#endif


#endif // IPXACTMODELS_GLOBAL_H
