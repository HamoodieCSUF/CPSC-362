import React, { useState } from 'react';
import { useUserStore } from '../../../../lib/userStore';
import { db } from '../../../../lib/firebase';
import { collection, doc, getDocs, query, where, setDoc, updateDoc, serverTimestamp } from 'firebase/firestore';

const AddUser = () => {
  const [username, setUsername] = useState('');
  const [user, setUser] = useState(null);
  const { currentUser } = useUserStore();

  const handleSearch = async (e) => {
    e.preventDefault();
    const q = query(collection(db, 'users'), where('username', '==', username));
    const querySnapshot = await getDocs(q);
    if (!querySnapshot.empty) {
      setUser(querySnapshot.docs[0].data());
    }
  };

  const handleAddUser = async () => {
    if (user && currentUser) {
      const combinedId = currentUser.id > user.id ? currentUser.id + user.id : user.id + currentUser.id;
      await setDoc(doc(db, 'chats', combinedId), { messages: [] });
      await updateDoc(doc(db, 'userChats', currentUser.id), {
        [`${combinedId}.userInfo`]: {
          id: user.id,
          username: user.username,
          avatar: user.avatar,
        },
        [`${combinedId}.date`]: serverTimestamp(),
      });
      await updateDoc(doc(db, 'userChats', user.id), {
        [`${combinedId}.userInfo`]: {
          id: currentUser.id,
          username: currentUser.username,
          avatar: currentUser.avatar,
        },
        [`${combinedId}.date`]: serverTimestamp(),
      });
    }
  };

  return (
    <div className="addUser">
      <form onSubmit={handleSearch}>
        <input type="text" placeholder="Username" value={username} onChange={(e) => setUsername(e.target.value)} />
        <button type="submit">Search</button>
      </form>
      {user && (
        <div>
          <p>{user.username}</p>
          <button onClick={handleAddUser}>Add User</button>
        </div>
      )}
    </div>
  );
};

export default AddUser;