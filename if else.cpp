#include <iostream>
/***
DE BAI : Neu ki tu nhap vao thuoc bang chu cai -> IN HOA
         Khong thuoc thi in ki tu do ra
***/
using namespace std;

int main() {
    char KiTu,ChuInHoa;
    cin>>KiTu;
    if (KiTu>=97 and KiTu<=122) //kiem tra thuoc bang chu cai bang ASCII
    {
        ChuInHoa=KiTu-32; //
        cout<<ChuInHoa;
    }
    else
    {
        cout<<KiTu;
    }

    return 0;
}
