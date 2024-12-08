import React from 'react';
import { render, screen, fireEvent, act } from '@testing-library/react';
import '@testing-library/jest-dom';
import AddUser from '../addUser';
import { useUserStore } from '../../../../../lib/userStore';
import { db } from '../../../../../lib/firebase';
import { collection, doc, getDocs, query, where, setDoc, updateDoc, serverTimestamp } from 'firebase/firestore';

jest.mock('../../../../../lib/userStore');
jest.mock('firebase/firestore');

describe('AddUser Component', () => {
  const mockUserStore = {
    currentUser: { id: 'currentUser1', username: 'currentuser', avatar: './currentuser.png' },
  };

  const mockUser = {
    id: 'user1',
    username: 'testuser',
    avatar: './avatar.png',
  };

  beforeEach(() => {
    useUserStore.mockReturnValue(mockUserStore);

    getDocs.mockResolvedValue({
      empty: false,
      docs: [{ data: () => mockUser }],
    });

    setDoc.mockResolvedValue();
    updateDoc.mockResolvedValue();
  });

  test('renders AddUser component', async () => {
    await act(async () => {
      render(<AddUser />);
    });
    expect(screen.getByPlaceholderText('Username')).toBeInTheDocument();
    expect(screen.getByText('Search')).toBeInTheDocument();
  });

  test('searches for a user', async () => {
    await act(async () => {
      render(<AddUser />);
    });
    const input = screen.getByPlaceholderText('Username');
    const button = screen.getByText('Search');

    await act(async () => {
      fireEvent.change(input, { target: { value: 'testuser' } });
      fireEvent.click(button);
    });

    expect(await screen.findByText('testuser')).toBeInTheDocument();
  });

  test('adds a user', async () => {
    await act(async () => {
      render(<AddUser />);
    });
    const input = screen.getByPlaceholderText('Username');
    const searchButton = screen.getByText('Search');

    await act(async () => {
      fireEvent.change(input, { target: { value: 'testuser' } });
      fireEvent.click(searchButton);
    });

    const addButton = await screen.findByText('Add User');

    await act(async () => {
      fireEvent.click(addButton);
    });

    expect(setDoc).toHaveBeenCalled();
    expect(updateDoc).toHaveBeenCalledTimes(2);
  });
});