//
//  Copyright (C) 2018-  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once
namespace DirectOutput {
    void onEnumerate(void *hDevice, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onEnumerate(hDevice);
    }

    void onDeviceChanged(void *hDevice, bool bAdded, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onDeviceChanged(hDevice, bAdded);

    }

    void onPageChange(void *hDevice, DWORD dwPage, bool bSetActive, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onPageChange(hDevice, dwPage, bSetActive);
    }

    void onSoftButtonChange(void *hDevice, DWORD dwButtons, void *pCtxt) {
        if(!pCtxt) { return; }
        reinterpret_cast<X52ProMFD *>(pCtxt)->onSoftButtonChange(hDevice, dwButtons);
    }
}
