// src/components/list/chatList/__tests__/Chatlist.test.js

import React from 'react';
import { render, screen, fireEvent, act } from '@testing-library/react';
import '@testing-library/jest-dom';
import ChatList from '../Chatlist';
import { useUserStore } from '../../../../lib/userStore';
import { useChatStore } from '../../../../lib/chatStore';
import { doc, getDoc, onSnapshot, updateDoc } from 'firebase/firestore';

jest.mock('../../../../lib/userStore');
jest.mock('../../../../lib/chatStore');
jest.mock('firebase/firestore');

describe('ChatList Component', () => {
  const mockUserStore = {
    currentUser: { id: 'user1' },
  };

  const mockChatStore = {
    chatId: 'chat1',
    changeChat: jest.fn(),
  };

  const mockChats = [
    {
      chatId: 'chat1',
      receiverId: 'user2',
      updatedAt: 1620000000000,
      lastMessage: 'Hello',
      isSeen: false,
      user: { username: 'user2', avatar: './avatar.png' },
    },
  ];

  beforeEach(() => {
    useUserStore.mockReturnValue(mockUserStore);
    useChatStore.mockReturnValue(mockChatStore);

    onSnapshot.mockImplementation((docRef, callback) => {
      callback({
        data: () => ({
          chats: mockChats,
        }),
      });
      return jest.fn(); // Mock the unsubscribe function
    });

    getDoc.mockResolvedValue({
      data: () => ({
        username: 'user2',
        avatar: './avatar.png',
      }),
    });

    updateDoc.mockResolvedValue();
  });

  test('renders ChatList component', () => {
    render(<ChatList />);
    expect(screen.getByPlaceholderText('Search')).toBeInTheDocument();
    expect(screen.getAllByAltText('')).toHaveLength(2); // Adjusted to use getAllByAltText
  });

  test('displays chats correctly', async () => {
    await act(async () => {
      render(<ChatList />);
    });
    const chatItem = await screen.findByText('Hello');
    expect(chatItem).toBeInTheDocument();
  });

  test('filters chats based on search input', async () => {
    await act(async () => {
      render(<ChatList />);
    });
    const searchInput = screen.getByPlaceholderText('Search');
    fireEvent.change(searchInput, { target: { value: 'user2' } });
    const chatItem = await screen.findByText('Hello');
    expect(chatItem).toBeInTheDocument();
  });

  test('handles chat selection', async () => {
    await act(async () => {
      render(<ChatList />);
    });
    const chatItem = await screen.findByText('Hello');
    await act(async () => {
      fireEvent.click(chatItem);
    });
    expect(mockChatStore.changeChat).toHaveBeenCalledWith('chat1', {
      username: 'user2',
      avatar: './avatar.png',
    });
  });
});