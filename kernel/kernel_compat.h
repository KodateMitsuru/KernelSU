#ifndef __KSU_H_KERNEL_COMPAT
#define __KSU_H_KERNEL_COMPAT

#include "linux/fs.h"
#include "linux/key.h"
#include "linux/version.h"
#include "linux/uaccess.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
#define ksu_strncpy_from_user_nofault strncpy_from_user_nofault
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0)
#define ksu_strncpy_from_user_nofault strncpy_from_unsafe_user
#else
#define ksu_strncpy_from_user_nofault strncpy_from_user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)
extern struct key *init_session_keyring;
#endif

extern void ksu_android_ns_fs_check();
extern struct file *ksu_filp_open_compat(const char *filename, int flags, umode_t mode);
extern ssize_t ksu_kernel_read_compat(struct file *p, void *buf, size_t count, loff_t *pos);
extern ssize_t ksu_kernel_write_compat(struct file *p, const void *buf, size_t count, loff_t *pos);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)
static inline int install_session_keyring(struct key *keyring)
{
	struct cred *new;
	int ret;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

	ret = install_session_keyring_to_cred(new, keyring);
	if (ret < 0) {
		abort_creds(new);
		return ret;
	}

	return commit_creds(new);
}
#define KWORKER_INSTALL_KEYRING()                           \
	static bool keyring_installed = false;                  \
	if (init_session_keyring != NULL && !keyring_installed) \
	{                                                       \
		install_session_keyring(init_session_keyring);      \
		keyring_installed = true;                           \
	}
#else
#define KWORKER_INSTALL_KEYRING()
#endif

#endif
